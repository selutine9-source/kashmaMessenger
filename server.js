'use strict';

/*
 * KashmaMessenger — backend
 * Чистый Node.js, без npm-зависимостей.
 * Аккаунты: логин + пароль (без email), scrypt-хэш, токен-сессии.
 * Сообщения: история в data/store.json, live-доставка через SSE.
 */

const http = require('http');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');
const { URL } = require('url');

const PORT = process.env.PORT ? Number(process.env.PORT) : 3000;
const HOST = process.env.HOST || '0.0.0.0';
const ROOT = __dirname;
const PUBLIC_DIR = path.join(ROOT, 'public');
const DATA_DIR = path.join(ROOT, 'data');
const DATA_FILE = path.join(DATA_DIR, 'store.json');
const MAX_MSG_PER_ROOM = 500;
const SESSION_TTL = 30 * 24 * 60 * 60 * 1000;
const MAX_TEXT = 2000;
const USER_RE = /^[a-zA-Z0-9_\-]{2,24}$/;

if (!fs.existsSync(DATA_DIR)) fs.mkdirSync(DATA_DIR, { recursive: true });
if (!fs.existsSync(DATA_FILE)) fs.writeFileSync(DATA_FILE, JSON.stringify({ users: {}, sessions: {}, messages: {} }));

let store;
try {
    store = JSON.parse(fs.readFileSync(DATA_FILE, 'utf8'));
} catch (e) {
    store = {};
}
store.users = store.users || {};
store.sessions = store.sessions || {};
store.messages = store.messages || {};

function save() {
    try { fs.writeFileSync(DATA_FILE, JSON.stringify(store)); } catch (e) {}
}

function json(res, code, obj) {
    const body = JSON.stringify(obj);
    res.writeHead(code, {
        'Content-Type': 'application/json; charset=utf-8',
        'Content-Length': Buffer.byteLength(body),
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Headers': 'Content-Type, Authorization',
        'Access-Control-Allow-Methods': 'GET, POST, OPTIONS'
    });
    res.end(body);
}

function readBody(req, limit) {
    limit = limit || 64 * 1024;
    return new Promise((resolve, reject) => {
        let size = 0;
        const chunks = [];
        req.on('data', (c) => {
            size += c.length;
            if (size > limit) { reject(new Error('too large')); req.destroy(); return; }
            chunks.push(c);
        });
        req.on('end', () => {
            if (!chunks.length) return resolve({});
            try { resolve(JSON.parse(Buffer.concat(chunks).toString('utf8'))); }
            catch (e) { reject(new Error('bad json')); }
        });
        req.on('error', reject);
    });
}

function hashPassword(password, salt) {
    const s = salt || crypto.randomBytes(16).toString('hex');
    const h = crypto.scryptSync(String(password), s, 64).toString('hex');
    return { salt: s, hash: h };
}

function verifyPassword(password, salt, hash) {
    const calc = crypto.scryptSync(String(password), salt, 64);
    const expected = Buffer.from(hash, 'hex');
    if (expected.length !== calc.length) return false;
    return crypto.timingSafeEqual(calc, expected);
}

function slug(s) {
    return String(s || '').toLowerCase().replace(/[^a-z0-9_\-]/g, '').slice(0, 32) || 'general';
}

function createSession(user) {
    const token = crypto.randomBytes(24).toString('hex');
    store.sessions[token] = { user: user, expires: Date.now() + SESSION_TTL };
    save();
    return token;
}

function authUser(req, query) {
    let token = null;
    const h = req.headers['authorization'];
    if (h && h.startsWith('Bearer ')) token = h.slice(7);
    if (!token && query) token = query.get('token');
    if (!token) return null;
    const s = store.sessions[token];
    if (!s) return null;
    if (s.expires < Date.now()) { delete store.sessions[token]; save(); return null; }
    return { token: token, user: s.user };
}

/* ---------- realtime (SSE) ---------- */

const clients = new Set();
let clientSeq = 0;

function usersInRoom(room) {
    const set = new Set();
    for (const c of clients) if (c.room === room) set.add(c.user);
    return Array.from(set);
}

function writeEvent(res, event, data) {
    try { res.write('event: ' + event + '\ndata: ' + data + '\n\n'); } catch (e) {}
}

function broadcastPresence(room) {
    const data = JSON.stringify({ room: room, users: usersInRoom(room) });
    for (const c of clients) if (c.room === room) writeEvent(c.res, 'presence', data);
}

function broadcastMessage(room, msg) {
    const data = JSON.stringify(msg);
    for (const c of clients) if (c.room === room) writeEvent(c.res, 'message', data);
}

function handleStream(req, res, query) {
    const room = slug(query.get('room') || 'general');
    res.writeHead(200, {
        'Content-Type': 'text/event-stream; charset=utf-8',
        'Cache-Control': 'no-cache, no-transform',
        'Connection': 'keep-alive',
        'X-Accel-Buffering': 'no',
        'Access-Control-Allow-Origin': '*'
    });
    res.write(': connected\n\n');

    const a = authUser(req, query);
    if (!a) {
        writeEvent(res, 'error', JSON.stringify({ error: 'auth' }));
        res.end();
        return;
    }

    const c = { id: ++clientSeq, res: res, user: a.user, room: room };
    clients.add(c);
    broadcastPresence(room);

    const ping = setInterval(() => { try { res.write(': ping\n\n'); } catch (e) {} }, 25000);
    req.on('close', () => {
        clearInterval(ping);
        clients.delete(c);
        broadcastPresence(room);
    });
}

/* ---------- API ---------- */

async function handleApi(req, res, u, pathname) {
    try {
        if (pathname === '/api/health' && req.method === 'GET') {
            return json(res, 200, { ok: true, mode: 'api' });
        }

        if (pathname === '/api/register' && req.method === 'POST') {
            const body = await readBody(req);
            const name = String(body.username || '').trim();
            const pass = String(body.password || '');
            if (!USER_RE.test(name)) return json(res, 400, { error: 'Ник: 2-24 символа, только латиница, цифры, _ и -' });
            if (pass.length < 4) return json(res, 400, { error: 'Пароль минимум 4 символа' });
            const key = name.toLowerCase();
            if (store.users[key]) return json(res, 409, { error: 'Такой ник уже занят' });
            const h = hashPassword(pass);
            store.users[key] = { name: name, salt: h.salt, hash: h.hash, created: Date.now() };
            const token = createSession(name);
            return json(res, 200, { token: token, user: name });
        }

        if (pathname === '/api/login' && req.method === 'POST') {
            const body = await readBody(req);
            const name = String(body.username || '').trim();
            const pass = String(body.password || '');
            const rec = store.users[name.toLowerCase()];
            if (!rec || !verifyPassword(pass, rec.salt, rec.hash))
                return json(res, 401, { error: 'Неверный ник или пароль' });
            const token = createSession(rec.name);
            return json(res, 200, { token: token, user: rec.name });
        }

        if (pathname === '/api/logout' && req.method === 'POST') {
            const a = authUser(req, u.searchParams);
            if (a) { delete store.sessions[a.token]; save(); }
            return json(res, 200, { ok: true });
        }

        if (pathname === '/api/me' && req.method === 'GET') {
            const a = authUser(req, u.searchParams);
            if (!a) return json(res, 401, { error: 'auth' });
            return json(res, 200, { user: a.user });
        }

        if (pathname === '/api/history' && req.method === 'GET') {
            const a = authUser(req, u.searchParams);
            if (!a) return json(res, 401, { error: 'auth' });
            const room = slug(u.searchParams.get('room') || 'general');
            return json(res, 200, { room: room, messages: store.messages[room] || [] });
        }

        if (pathname === '/api/send' && req.method === 'POST') {
            const a = authUser(req, u.searchParams);
            if (!a) return json(res, 401, { error: 'auth' });
            const body = await readBody(req);
            const room = slug(body.room || 'general');
            let text = String(body.text || '').trim();
            if (!text) return json(res, 400, { error: 'Пустое сообщение' });
            if (text.length > MAX_TEXT) text = text.slice(0, MAX_TEXT);
            const msg = { id: crypto.randomBytes(8).toString('hex'), u: a.user, t: text, ts: Date.now() };
            const arr = store.messages[room] || (store.messages[room] = []);
            arr.push(msg);
            if (arr.length > MAX_MSG_PER_ROOM) arr.splice(0, arr.length - MAX_MSG_PER_ROOM);
            save();
            broadcastMessage(room, msg);
            return json(res, 200, { message: msg });
        }

        return json(res, 404, { error: 'not found' });
    } catch (e) {
        return json(res, 400, { error: 'Некорректный запрос' });
    }
}

/* ---------- static ---------- */

const MIME = {
    '.html': 'text/html; charset=utf-8',
    '.css': 'text/css; charset=utf-8',
    '.js': 'application/javascript; charset=utf-8',
    '.json': 'application/json; charset=utf-8',
    '.svg': 'image/svg+xml',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.ico': 'image/x-icon',
    '.txt': 'text/plain; charset=utf-8'
};

function serveStatic(req, res, pathname) {
    if (pathname === '/') pathname = '/index.html';
    const safe = path.normalize(pathname).replace(/^(\.\.[\\/])+/, '');
    const filePath = path.join(PUBLIC_DIR, safe);
    if (!filePath.startsWith(PUBLIC_DIR)) {
        res.writeHead(403, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Forbidden');
        return;
    }
    fs.readFile(filePath, (err, data) => {
        if (err) {
            res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Not found');
            return;
        }
        res.writeHead(200, { 'Content-Type': MIME[path.extname(filePath).toLowerCase()] || 'application/octet-stream' });
        res.end(data);
    });
}

/* ---------- server ---------- */

const server = http.createServer((req, res) => {
    const u = new URL(req.url, 'http://' + (req.headers.host || 'localhost'));
    const pathname = decodeURIComponent(u.pathname);

    if (req.method === 'OPTIONS') {
        res.writeHead(204, {
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Headers': 'Content-Type, Authorization',
            'Access-Control-Allow-Methods': 'GET, POST, OPTIONS'
        });
        res.end();
        return;
    }

    if (pathname === '/api/stream') return handleStream(req, res, u.searchParams);
    if (pathname.indexOf('/api/') === 0) return handleApi(req, res, u, pathname);
    serveStatic(req, res, pathname);
});

server.listen(PORT, HOST, () => {
    console.log('');
    console.log('  KashmaMessenger сервер запущен');
    console.log('  ---------------------------------');
    console.log('  Локально:  http://localhost:' + PORT);
    console.log('  В сети:    http://<IP-этого-ПК>:' + PORT);
    console.log('  Данные:    ' + DATA_FILE);
    console.log('');
});
