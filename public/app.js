(function () {
    "use strict";

    /*
     * Бэкенд выбирается автоматически:
     *  - если доступен Node-сервер (/api/health) — режим "api";
     *  - иначе (например, GitHub Pages) — режим "ntfy" через публичный relay ntfy.sh.
     * Адрес Node-сервера можно задать: window.KASHMA_API_BASE = "https://сервер".
     */

    var API_BASE = window.KASHMA_API_BASE || "";
    if (!API_BASE) API_BASE = location.protocol.indexOf("http") === 0 ? location.origin : "http://localhost:3000";
    API_BASE = API_BASE.replace(/\/$/, "");
    var NTFY = "https://ntfy.sh";

    var TOKEN_KEY = "kashma_token";
    var PROFILE_KEY = "kashma_relay_profile";
    var ROOMS = ["general", "kashma", "offtop"];

    var MODE = null;
    var token = null;
    var me = null;
    var room = "general";
    var stream = null;
    var pressStream = null;
    var roomSecret = "";
    var authMode = "login";
    var seen = {};

    var $ = function (id) { return document.getElementById(id); };

    function setStatus(state) {
        var dot = $("connDot"), txt = $("connText");
        if (state === "on") { dot.className = "dot on"; txt.textContent = "в сети"; }
        else if (state === "connecting") { dot.className = "dot"; txt.textContent = "подключение..."; }
        else { dot.className = "dot off"; txt.textContent = "оффлайн"; }
    }

    /* ---------- utils ---------- */

    function slug(s) {
        return String(s || "").toLowerCase().replace(/[^a-z0-9_\-]/g, "").slice(0, 24) || "user";
    }

    function fnv(str, seed) {
        var h = seed >>> 0;
        for (var i = 0; i < str.length; i++) {
            h ^= str.charCodeAt(i);
            h = Math.imul(h, 16777619) >>> 0;
        }
        return h >>> 0;
    }

    function relayHash(s) {
        s = String(s);
        return (fnv(s, 2166136261).toString(36) + fnv(s + "#2", 2246822507).toString(36) + fnv(s + "#3", 3266489909).toString(36));
    }

    function roomSecretFor(pass) {
        pass = String(pass || "");
        return pass ? relayHash("kashma-room|" + pass).slice(0, 20) : "";
    }

    function topicFor(r) { return "kashma-" + (roomSecret ? roomSecret + "-" : "") + slug(r); }
    function presenceFor(r) { return topicFor(r) + "-p"; }

    function fmtTime(ts) {
        var d = new Date(ts);
        return ("0" + d.getHours()).slice(-2) + ":" + ("0" + d.getMinutes()).slice(-2);
    }

    /* ---------- UI ---------- */

    function showChat() {
        $("authScreen").classList.add("hidden");
        $("chatScreen").classList.remove("hidden");
        $("logoutBtn").classList.remove("hidden");
        $("roomLabel").textContent = "#" + room;
        if (MODE === "ntfy") $("connText").textContent = "тема: " + topicFor(room);
    }

    function showAuth() {
        $("chatScreen").classList.add("hidden");
        $("authScreen").classList.remove("hidden");
        $("logoutBtn").classList.add("hidden");
        $("roomLabel").textContent = "";
    }

    function syncCallHref() {
        var el = $("navCalls");
        if (el) el.href = "calls.html?room=" + encodeURIComponent(room);
    }

    function clearMessages() {
        $("messages").innerHTML = '<div class="empty">Пока сообщений нет. Напиши первым!</div>';
    }

    function addMessage(msg) {
        if (!msg || typeof msg.t !== "string") return;
        if (msg.id && seen[msg.id]) return;
        if (msg.id) seen[msg.id] = 1;
        var box = $("messages");
        var empty = box.querySelector(".empty");
        if (empty) empty.remove();
        var own = msg.u === me;
        var wrap = document.createElement("div");
        wrap.className = "msg" + (own ? " own" : "");
        var meta = document.createElement("div");
        meta.className = "meta";
        meta.textContent = (own ? "Ты" : (msg.u || "аноним")) + " · " + fmtTime(msg.ts || Date.now());
        var bubble = document.createElement("div");
        bubble.className = "bubble";
        bubble.textContent = msg.t;
        wrap.appendChild(meta);
        wrap.appendChild(bubble);
        box.appendChild(wrap);
        box.scrollTop = box.scrollHeight;
        while (box.children.length > 500) box.removeChild(box.firstChild);
    }

    function addSystem(text) {
        var box = $("messages");
        var empty = box.querySelector(".empty");
        if (empty) empty.remove();
        var wrap = document.createElement("div");
        wrap.className = "msg system";
        var bubble = document.createElement("div");
        bubble.className = "bubble";
        bubble.textContent = text;
        wrap.appendChild(bubble);
        box.appendChild(wrap);
        box.scrollTop = box.scrollHeight;
    }

    function renderRooms() {
        var list = $("roomList");
        list.innerHTML = "";
        var rooms = ROOMS.slice();
        if (rooms.indexOf(room) === -1) rooms.unshift(room);
        rooms.forEach(function (r) {
            var b = document.createElement("button");
            b.type = "button";
            b.className = "room-btn" + (r === room ? " active" : "");
            b.textContent = "#" + r;
            b.addEventListener("click", function () { switchRoom(r); });
            list.appendChild(b);
        });
    }

    function renderUsers(users) {
        var list = $("userList");
        list.innerHTML = "";
        users = (users || []).filter(function (v, i, a) { return a.indexOf(v) === i; });
        users.forEach(function (nick) {
            var row = document.createElement("div");
            row.className = "user-row";
            var dot = document.createElement("span");
            dot.className = "dot on";
            var name = document.createElement("span");
            name.textContent = nick + (nick === me ? " (ты)" : "");
            row.appendChild(dot);
            row.appendChild(name);
            list.appendChild(row);
        });
        $("userCount").textContent = users.length;
    }

    /* ---------- api backend ---------- */

    function api(path, opts) {
        opts = opts || {};
        opts.headers = opts.headers || {};
        if (token) opts.headers["Authorization"] = "Bearer " + token;
        if (opts.body && typeof opts.body !== "string") {
            opts.headers["Content-Type"] = "application/json";
            opts.body = JSON.stringify(opts.body);
        }
        return fetch(API_BASE + path, opts).then(function (r) {
            return r.json().catch(function () { return {}; }).then(function (data) {
                if (!r.ok) throw new Error(data.error || ("HTTP " + r.status));
                return data;
            });
        });
    }

    var ApiBackend = {
        auth: function () {
            var nick = $("nick").value.trim();
            var pass = $("pass").value;
            return api("/api/" + authMode, { method: "POST", body: { username: nick, password: pass } })
                .then(function (data) {
                    token = data.token;
                    try { localStorage.setItem(TOKEN_KEY, token); } catch (e) {}
                    return data.user;
                });
        },
        restore: function () {
            try { token = localStorage.getItem(TOKEN_KEY); } catch (e) {}
            if (!token) return Promise.resolve(null);
            return api("/api/me").then(function (d) { return d.user; }).catch(function () {
                token = null;
                try { localStorage.removeItem(TOKEN_KEY); } catch (e) {}
                return null;
            });
        },
        history: function (r) {
            return api("/api/history?room=" + encodeURIComponent(r)).then(function (d) { return d.messages || []; });
        },
        send: function (r, text) {
            return api("/api/send", { method: "POST", body: { room: r, text: text } });
        },
        logout: function () { return api("/api/logout", { method: "POST" }).catch(function () {}); },
        subscribe: function (r, onMsg, onPresence, onState) {
            closeStream();
            onState("connecting");
            stream = new EventSource(API_BASE + "/api/stream?room=" + encodeURIComponent(r) + "&token=" + encodeURIComponent(token));
            stream.onopen = function () { onState("on"); };
            stream.onerror = function () { onState("off"); };
            stream.addEventListener("message", function (e) {
                try { onMsg(JSON.parse(e.data)); } catch (err) {}
            });
            stream.addEventListener("presence", function (e) {
                try { onPresence(JSON.parse(e.data).users); } catch (err) {}
            });
        },
        heartbeat: function () {}
    };

    /* ---------- ntfy relay backend ---------- */

    var ntfyUsers = {};
    var ntfyTimer = null;

    function closeStream() {
        if (stream) { stream.close(); stream = null; }
        if (pressStream) { pressStream.close(); pressStream = null; }
        if (ntfyTimer) { clearInterval(ntfyTimer); ntfyTimer = null; }
    }

    function ntfyPublish(topic, body, cache) {
        return fetch(NTFY + "/" + topic, {
            method: "POST",
            headers: { "Cache": cache ? "yes" : "no" },
            body: body
        });
    }

    var NtfyBackend = {
        auth: function () {
            var nick = $("nick").value.trim();
            var pass = $("pass").value;
            roomSecret = roomSecretFor(pass);
            try { localStorage.setItem(PROFILE_KEY, JSON.stringify({ u: nick, s: roomSecret })); } catch (e) {}
            return Promise.resolve(nick);
        },
        restore: function () {
            var saved = null;
            try { saved = JSON.parse(localStorage.getItem(PROFILE_KEY)); } catch (e) {}
            if (!saved || !saved.u) return Promise.resolve(null);
            roomSecret = saved.s || "";
            return Promise.resolve(saved.u);
        },
        history: function (r) {
            return Promise.resolve([]);
        },
        send: function (r, text) {
            var msg = { id: Math.random().toString(36).slice(2, 10), u: me, t: text.slice(0, 3500), ts: Date.now() };
            return ntfyPublish(topicFor(r), JSON.stringify(msg), true);
        },
        logout: function () {
            try { localStorage.removeItem(PROFILE_KEY); } catch (e) {}
            return Promise.resolve();
        },
        subscribe: function (r, onMsg, onPresence, onState) {
            closeStream();
            ntfyUsers = {};
            onState("connecting");

            stream = new EventSource(NTFY + "/" + topicFor(r) + "/sse?since=12h");
            stream.onopen = function () { onState("on"); };
            stream.onerror = function () { onState("off"); };
            stream.onmessage = function (e) {
                var w;
                try { w = JSON.parse(e.data); } catch (err) { return; }
                if (w.event !== "message") return;
                try { onMsg(JSON.parse(w.message)); } catch (err) {}
            };

            pressStream = new EventSource(NTFY + "/" + presenceFor(r) + "/sse");
            pressStream.onmessage = function (e) {
                var w, p;
                try { w = JSON.parse(e.data); } catch (err) { return; }
                if (w.event !== "message") return;
                try { p = JSON.parse(w.message); } catch (err) { return; }
                if (p && p.u) { ntfyUsers[p.u] = Date.now(); }
                onPresence(presentUsers());
            };

            this.heartbeat();
        },
        heartbeat: function () {
            var tick = function () {
                if (!me) return;
                ntfyPublish(presenceFor(room), JSON.stringify({ u: me, ts: Date.now() }), false)
                    .then(function () { renderUsers(presentUsers()); })
                    .catch(function () {});
            };
            tick();
            ntfyTimer = setInterval(tick, 20000);
        }
    };

    function presentUsers() {
        var now = Date.now();
        var arr = [];
        Object.keys(ntfyUsers).forEach(function (u) {
            if (now - ntfyUsers[u] > 46000) { delete ntfyUsers[u]; return; }
            arr.push(u);
        });
        return arr;
    }

    /* ---------- shared chat flow ---------- */

    function backend() { return MODE === "ntfy" ? NtfyBackend : ApiBackend; }

    function loadHistory() {
        clearMessages();
        seen = {};
        return backend().history(room).then(function (msgs) {
            msgs.forEach(addMessage);
        }).catch(function () {});
    }

    function openStream() {
        backend().subscribe(room, addMessage, renderUsers, setStatus);
    }

    function switchRoom(r) {
        if (r === room) return;
        room = r;
        $("roomLabel").textContent = "#" + room;
        syncCallHref();
        renderRooms();
        renderUsers([]);
        closeStream();
        loadHistory().then(function () {
            addSystem("Комната #" + room);
            openStream();
        });
    }

    function enterChat(user) {
        me = user;
        room = "general";
        syncCallHref();
        showChat();
        renderRooms();
        loadHistory().then(function () {
            if (MODE === "ntfy") addSystem("Комната #" + room + ". Код темы: " + topicFor(room) + " — у тебя и собеседника он должен быть одинаковым (для этого одинаковый пароль комнаты или пустой).");
            openStream();
        });
    }

    function doAuth() {
        var nick = $("nick").value.trim();
        var pass = $("pass").value;
        $("authErr").textContent = "";
        if (nick.length < 2) { $("authErr").textContent = "Ник минимум 2 символа."; return; }
        if (MODE === "api") {
            if (pass.length < 4) { $("authErr").textContent = "Пароль минимум 4 символа."; return; }
        } else if (pass.length > 0 && pass.length < 4) {
            $("authErr").textContent = "Пароль комнаты — минимум 4 символа или пусто."; return;
        }
        if (authMode === "register" && pass !== $("pass2").value) {
            $("authErr").textContent = "Пароли не совпадают."; return;
        }
        $("authBtn").disabled = true;
        backend().auth().then(function (user) {
            $("pass").value = ""; $("pass2").value = "";
            enterChat(user);
        }).catch(function (err) {
            $("authErr").textContent = err.message;
        }).then(function () { $("authBtn").disabled = false; });
    }

    function logout() {
        backend().logout();
        closeStream();
        token = null; me = null;
        try { localStorage.removeItem(TOKEN_KEY); } catch (e) {}
        renderUsers([]);
        showAuth();
        setStatus("off");
    }

    /* ---------- events ---------- */

    Array.prototype.forEach.call(document.querySelectorAll(".tab"), function (tab) {
        tab.addEventListener("click", function () {
            authMode = tab.getAttribute("data-tab");
            Array.prototype.forEach.call(document.querySelectorAll(".tab"), function (t) { t.classList.remove("active"); });
            tab.classList.add("active");
            var reg = authMode === "register";
            $("pass2Field").classList.toggle("hidden", !reg);
            $("authBtn").textContent = reg ? "Зарегистрироваться" : "Войти";
            $("pass").setAttribute("autocomplete", reg ? "new-password" : "current-password");
            $("authErr").textContent = "";
        });
    });

    $("authForm").addEventListener("submit", function (e) { e.preventDefault(); doAuth(); });

    $("composer").addEventListener("submit", function (e) {
        e.preventDefault();
        var input = $("msgInput");
        var text = input.value.trim();
        if (!text) return;
        input.value = "";
        backend().send(room, text).catch(function (err) {
            addSystem("Ошибка отправки: " + (err.message || err));
        });
    });

    $("logoutBtn").addEventListener("click", logout);

    /* ---------- start ---------- */

    function detectBackend() {
        if (window.KASHMA_BACKEND === "ntfy" || window.KASHMA_BACKEND === "api") {
            return Promise.resolve(window.KASHMA_BACKEND);
        }
        var ctrl = new AbortController();
        var timer = setTimeout(function () { ctrl.abort(); }, 4000);
        return fetch(API_BASE + "/api/health", { signal: ctrl.signal })
            .then(function (r) { clearTimeout(timer); return r.ok ? "api" : "ntfy"; })
            .catch(function () { clearTimeout(timer); return "ntfy"; });
    }

    $("authBtn").disabled = true;
    $("authErr").textContent = "Определяю бэкенд...";
    detectBackend().then(function (mode) {
        MODE = mode;
        if (mode === "ntfy") {
            $("authSub").textContent = "Общий пароль комнаты: у кого пароль совпадает (или у всех пусто) — те видят друг друга. Ник — просто подпись.";
            $("passLabel").textContent = "Пароль комнаты (общий)";
            $("pass").setAttribute("placeholder", "общий; пусто — публичная комната");
        }
        $("authBtn").disabled = false;
        $("authErr").textContent = "";
        return backend().restore();
    }).then(function (user) {
        if (user) enterChat(user);
        else setStatus("off");
    }).catch(function () {
        $("authBtn").disabled = false;
        $("authErr").textContent = "Не удалось определить бэкенд.";
    });
})();
