// zimeimpl.js

var RomanParser = Class.extend(Parser, {

    initialize: function (schema) {
        this._alphabet = schema.alphabet;
        this._initial = schema.initial;
        this._delimiter = schema.delimiter;
        this._xform = schema.xform;
        this._input = [];
    },
    
    clear: function () {
        this._input.length = 0;
    },
    
    isEmpty: function () {
        return this._input.length == 0;
    },
    
    _getInput: function() {
        if (this._xform) {
            var s = this._input.join("");
            return this._xform(s).split("");
        }
        else {
            return this._input.slice(0);
        }
    },
    
    _isInput: function (ch, empty) {
        if (empty) {
            return this._initial.indexOf(ch) != -1;
        } else {
            return this._alphabet.indexOf(ch) != -1 || this._delimiter.indexOf(ch) != -1;
        }
    },
    
    processInput: function (event, ctx) {
        if (event.type == "keyup") {
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            return false;
        }
        var ch = KeyEvent.toChar(event);
        if (ctx.beingConverted()) {
            if (ctx.isCompleted() && ch != null && this._isInput(ch, true)) {
                this.clear();
                this._input.push(ch);
                return {type: "edit", value: this._getInput()};
            }
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_ESCAPE) {
            this.clear();
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_BACKSPACE) {
            if (this.isEmpty())
                return false;
            this._input.pop();
            ctx.input = this._getInput();
            return {type: "edit", value: null};
        }
        if (ch != null && this._isInput(ch, this.isEmpty())) {
            this._input.push(ch);
            ctx.input = this._getInput();
            return {type: "edit", value: null};
        }
        return false;
    }
    
});

Parser.register("roman", RomanParser);

var TableParser = Class.extend(Parser, {
    
    initialize: function (schema) {
        this._autoCommitKeywordLength = Number(schema.getConfigValue("AutoCommitKeywordLength") || 
                                               schema.getConfigValue("MaxKeywordLength") || 4);
        this._alphabet = schema.alphabet;
        this._initial = schema.initial;
        this._delimiter = schema.delimiter;
        this._xform = schema.xform;
        this.clear();
    },

    clear: function () {
        this._input = [];
        this._keyword = [];
    },

    isEmpty: function () {
        return this._input.length == 0 && this._keyword.length == 0;
    },

    _isKeywordEmpty: function () {
        return this._keyword.length == 0;
    },

    _getKeyword: function () {
        var k = this._keyword.join("");
        return this._xform ? this._xform(k) : k;
    },
    
    processInput: function (event, ctx) {
        if (event.type == "keyup") {
            return false;
        }
        var ch = KeyEvent.toChar(event);
        if (ctx.beingConverted()) {
            if (ctx.isCompleted() && ch != null && this._initial.indexOf(ch) != -1) {
                this.clear();
                this._keyword.push(ch);
                return {type: "edit", value: this._getKeyword()};
            }
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_ESCAPE) {
            this.clear();
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_BACKSPACE) {
            if (this.isEmpty())
                return false;
            if (this._isKeywordEmpty())
                this._keyword = this._input.pop();
            this._keyword.pop();
            if (this._isKeywordEmpty()) {
                ctx.popInput();
            }
            else {
                ctx.input.pop();
                ctx.input.push(this._getKeyword());
            }
            return {type: "edit", value: null};
        }
        if (ch == null) {
            return false;
        }
        if (!this.isEmpty() && this._delimiter.indexOf(ch) != -1) {
            if (!this._isKeywordEmpty())
                this._input.push(this._keyword);
            this._input.push([ch]);
            this._keyword = [];
            return {type: "edit", value: [ch]};
        }
        if (this._initial.indexOf(ch) != -1) {
            var complete = (this._keyword.length == this._autoCommitKeywordLength);
            if (complete) {
                this._input.push(this._keyword);
                this._keyword = [];
            }
            if (this._isKeywordEmpty()) {
                this._keyword.push(ch);
                var result = [];
                var last = this._input.length - 1;
                if (last >= 0 && this._delimiter.indexOf(this._input[last][0]) == -1)
                    result.push(this._delimiter.charAt(0));
                result.push(this._getKeyword());
                return {type: "edit", value: result};
            }
        }
        if (!this._isKeywordEmpty()) {
            if (this._alphabet.indexOf(ch) != -1) {
                this._keyword.push(ch);
                ctx.input.pop();
                return {type: "edit", value: this._getKeyword()};
            }
        }
        return false;
    }

});

Parser.register("table", TableParser);

var GroupingParser = Class.extend(Parser, {

    initialize: function (schema) {
        this._delimiter = schema.delimiter.charAt(0);
        this._promptPattern = schema.getConfigCharSequence("PromptPattern") || "%s\u203a";
        this._keyGroups = schema.getConfigValue("KeyGroups").split(/\s+/);
        this._codeGroups = schema.getConfigValue("CodeGroups").split(/\s+/);
        this._groupCount = this._keyGroups.length;
        this.clear();
    },
    
    clear: function () {
        this._slots = [];
        this._cursor = 0;
    },

    isEmpty: function () {
        return this._slots.length == 0;
    },

    _getKeyword: function () {
         return $.grep(this._slots, function (e) { return e; }).join("");
    },

    _getPrompt: function (first) {
        var promptText = this._promptPattern.replace(/%s/, this._getKeyword());
        if (first)
            return {type: "prompt", value: promptText};
        else
            return {type: "prompt", value: this._delimiter + promptText, start: this._delimiter.length};
    },

    _commitInput: function (first) {
        var keyword = this._getKeyword();
        this.clear();
        if (first)
            return {type: "edit", value: [keyword]};
        else
            return {type: "edit", value: [this._delimiter, keyword]};
    },

    processInput: function (event, ctx) {
        if (event.type == "keyup") {
            return false;
        }
        if (ctx.beingConverted()) {
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_ESCAPE) {
            this.clear();
            return false;
        }
        if (event.keyCode == KeyEvent.KEY_BACKSPACE) {
            if (this.isEmpty())
                return false;
            // delete the last symbol from current keyword
            var j = this._slots.length;
            do {
                --j;
            }
            while (j > 0 && !this._slots[j - 1]);
            this._slots.length = j;
            this._cursor = j;
            if (!this.isEmpty()) {
                return this._getPrompt(ctx.isEmpty());
            }
            else {
                // keyword disposed
                return {type: "prompt", value: null};
            }
        }
        if (event.keyCode == KeyEvent.KEY_ENTER) {
            // no commits while editing keyword
            return !this.isEmpty();
        }
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            if (this.isEmpty())
                return false;
            return this._commitInput(ctx.isEmpty());
        }
        // handle grouping input
        var ch = KeyEvent.toChar(event);
        if (!ch) {
            return false;
        }
        var k = this._cursor;
        var i;
        while ((i = this._keyGroups[k].indexOf(ch)) == -1) {
            if (++k >= this._groupCount)
                k = 0;
            if (k == this._cursor) {
                // not accepted
                return !this.isEmpty();
            }
        }
        // update current keyword
        this._slots[k] = this._codeGroups[k].charAt(i);
        if (++k >= this._groupCount) {
            return this._commitInput(ctx.isEmpty());
        }
        else {
            this._cursor = k;
            return this._getPrompt(ctx.isEmpty());
        }
    }

});

Parser.register("grouping", GroupingParser);

var ComboParser = Class.extend(Parser, {

    initialize: function (schema) {
        this._promptPattern = schema.getConfigCharSequence("PromptPattern") || "%s\u203a";
        this._delimiter = schema.delimiter.charAt(0);
        this._comboKeys = schema.getConfigCharSequence("ComboKeys") || "";
        this._comboCodes = schema.getConfigCharSequence("ComboCodes") || "";
        this._comboMaxLength = Math.min(this._comboKeys.length, this._comboCodes.length);
        this._comboSpace = schema.getConfigValue("ComboSpace") || "_";
        this._xform = schema.xform;
        this.clear();
    },

    clear: function () {
        this._combo = {};
        this._held = {};
        this._held.count = 0;
    },

    isEmpty: function () {
        return this._held.count == 0; 
    },

    _getComboString: function () {
        var codes = [];
        for (var i = 0; i < this._comboMaxLength; ++i) {
            if (this._combo[this._comboKeys.charAt(i)]) {
                codes.push(this._comboCodes.charAt(i));
            }
        }
        var s = codes.join("");
        return this._xform ? this._xform(s) : s;
    },

    _getPrompt: function (first) {
        var promptText = this._promptPattern.replace(/%s/, this._getComboString());
        if (first)
            return {type: "prompt", value: promptText};
        else
            return {type: "prompt", value: this._delimiter + promptText, start: this._delimiter.length};
    },

    _commitCombo: function (first) {
        var keyword = this._getComboString();
        this.clear();
        //Logger.debug("_commitCombo: [" + keyword + "]");
        if (keyword == this._comboSpace) {
            return {type: "keyevent", value: {type: "keydown", keyCode: KeyEvent.KEY_SPACE}};
        }
        if (!keyword) {
            return {type: "prompt", value: null};
        }
        return {type: "edit", value: first ? [keyword] : [this._delimiter, keyword]};
    },

    processInput: function (event, ctx) {
        var ch = KeyEvent.toChar(event);
        if (event.type == "keyup") {
            if (this._held[ch]) {
                this._held[ch] = false;
                --this._held.count;
                //Logger.debug("released: [" + ch + "]");
                if (this.isEmpty()) {
                    return this._commitCombo(ctx.isEmpty());
                }
                return true;
            }
            // non-combo keys
            return false;
        }
        if (this._comboKeys.indexOf(ch) != -1) {
            this._combo[ch] = true;
            this._held[ch] = true;
            ++this._held.count;
            //Logger.debug("held: [" + ch + "]");
            return this._getPrompt(ctx.isEmpty());
        }
        // non-combo keys
        if (!this.isEmpty()) {
            this.clear();
            return {type: "prompt", value: null};
        }
        return false;
    }
});

Parser.register("combo", ComboParser);

var _segmentation = function (schema, input) {
    var n = input.length;
    m = 0;
    var p = [];
    var a = [];
    for (var i = 0; i <= n; ++i) {
        a[i] = [];
    }
    allowDivide = function (i, j, s) {
        var flag = true;
        for (var k = 0; k < p.length; ++k) {
            if (!a[j][p[k]]) {
                if (flag && a[i][p[k]])
                    return true;
                else
                    continue;
            }
            // TODO: return true if suggested by divide rules
            // var lw = input.slice(p[k], j).join();
            // ...
            flag = false;
        }
        return false;
    };
    var q = [0];
    while (q.length > 0) {
        var i = q.shift();
        if (i == n) {
            p.push(i);
            break;
        }
        var ok = false;
        for (var j = i + 1; j <= n && j <= i + schema.maxKeywordLength; ++j) {
            var s = input.slice(i, j).join("");
            if (!schema.spellingMap[s]) {
                continue;
            }
            var t = (j < n && schema.delimiter.indexOf(input[j]) != -1) ? j + 1 : j;
            //Logger.debug("[" + i + ", " + j + ") " + s);
            var found = false;
            $.each(q, function (i_, e) {
                if (e == t) {
                    found = true;
                    return false;
                }
            });
            if (!found) {
                q.push(t);
                m = Math.max(m, t);
            }
            else if (!allowDivide(i, t, s)) {
                //Logger.debug("denied division");
                continue;
            }
            a[t][i] = schema.spellingMap[s];
            ok = true;
        }
        if (ok) {
            p.push(i);
        }
        q.sort(function (a, b) { return a - b; });
    }
    if (m < n)
        p.push(m);
    //Logger.debug("p: " + p);
    b = [];
    d = [];
    // path finding
    for (var i = p.length - 1; i >= 0; --i) {
        var ok = (p[i] == m);
        for (var j = 0; j < b.length; ++j) {
            if (p[i] < b[j] && a[b[j]][p[i]]) {
                ok = true;
                d = $.grep(d, function (e) {
                    return e >= b[j];
                });
            }
        }
        if (ok) {
            b.push(p[i]);
            d.push(p[i]);
        }
        else {
            a[p[i]].length = 0;
            //Logger.debug("removed edges starting at " + p[i]);
        }
    }
    b.reverse();
    d.reverse();
    return {n: n, m: m, a: a, b: b, d: d};
};

var JSONFileBackend = Class.extend(Backend, {

    DATA_DIR: "json/",
    SCHEMA_LIST: "SchemaList.json",
    CONFIG: "Config.json",
    SEPARATOR: "/",
    JSON: ".json",

    loadSchemaList: function (callback) {
        $.getJSON(this.DATA_DIR + this.SCHEMA_LIST, null, callback);
    },

    loadSchema: function (schemaName, callback, _sync) {
        var me = this;
        this._loadConfig(schemaName, function (config) {
            schema = new Schema (schemaName, config);
            me._loadDict(schema, _sync ? callback : null);
            if (!_sync) {
                callback(schema);
            }
        });
    },

    _loadConfig: function (schemaName, callback) {
        $.getJSON(this.DATA_DIR + schemaName + this.CONFIG, null, function(config) {
            for (var k in config.ioMap) {
                var s = {};
                $.each(config.ioMap[k], function(i, e) {
                    s[e] = true;
                });
                config.ioMap[k] = s;
            }
            callback(config);
        });
    },

    _loadDict: function (schema, callback) {
        var me = this;
        var dict_prefix = schema.dict;
        $.getJSON(this.DATA_DIR + dict_prefix + this.JSON, null, function (data) {
            data.dict_prefix = dict_prefix;
            me._dict = data;
            if (callback) {
                callback(schema);
            }
        });
    },

    segmentation: _segmentation,

    query: function (ctx, callback) {
        if (!this._dict) {
            Logger.error("dict not yet loaded.");
            return;
        }
        this._lookup(this._indexQueries(ctx), ctx, callback);
    },

    _indexQueries: function (ctx) {
        var maxKeywordLength = ctx.schema.maxKeywordLength;
        var maxKeyLength = ctx.schema.maxKeyLength;
        var a = ctx._segmentation.a;
        var b = ctx._segmentation.b;
        var queries = {};
        var q = [];
        for (var i = 0; i < b.length; ++i) {
            var s = [];
            for (var j = i + 1; j < b.length && b[j] - b[i] <= maxKeywordLength; ++j) {
                var kw = a[b[j]][b[i]];
                if (kw) {
                    var r = {ikey: [kw], start: b[i], end: b[j]};
                    this._registerQuery(queries, r);
                    if (r.ikey.length < maxKeyLength)
                        s.push(r);
                    var me = this;
                    $.each(q, function (k, e) {
                        if (e.end == b[i]) {
                            r = {ikey: e.ikey.concat([kw]), start: e.start, end: b[j]};
                            me._registerQuery(queries, r);
                            if (r.ikey.length < maxKeyLength)
                                s.push(r);
                        }
                    });
                }
            }
            q = q.concat(s);
        }
        return queries;
    },

    _registerQuery: function (queries, r) {
        //Logger.debug("registerQuery: [" + r.start + ", " + r.end + ") " + r.ikey);
        var index = r.ikey.slice(0, this._dict.indexingLevel).join("_");
        if (queries[index])
            queries[index].push(r);
        else
            queries[index] = [r];
    },

    _lookup: function (queries, ctx, callback) {
        var dict_prefix = this._dict.dict_prefix;
        var files = this._dict.files;
        var me = this;
        var pending = ctx.pending = [];
        // shared by all requests
        var phrase = [];
        for (var k in queries) {
            (function (index) {
                //Logger.debug("requesting: " + index);
                var url = me.DATA_DIR + dict_prefix + me.SEPARATOR + files[index] + me.JSON;
                var request = $.getJSON(url, null, function (data) {
                    //Logger.debug("fetched: " + index);
                    if (ctx.pending !== pending) {
                        //Logger.debug("lookup result discarded: " + index);
                        return;
                    }
                    // generate query results
                    $.each(queries[index], function (i, q) {
                        var a = phrase[q.start];
                        if (!a) {
                            a = phrase[q.start] = [];
                        }
                        // filter results
                        $.each(data[q.ikey.join(" ")] || [], function (i, e) {
                            var r = me._checkEntry(e, q, ctx);
                            if (r) {
                                if (!a[r.end])
                                    a[r.end] = [r];
                                else
                                    a[r.end].push(r);
                            }
                        });
                    });
                    var pos = -1;
                    $.each(pending, function (i, p) {
                        if (p.index == index) {
                            pos = i;
                            return false;
                        }
                    });
                    if (pos == -1) {
                        Logger.error("index '" + index + "' lost trace in pending lookups.");
                        return;
                    }
                    pending.splice(pos, 1);
                    if (pending.length == 0) {
                        ctx.error = null;
                        ctx.pending = null;
                        //Logger.debug("lookup successful.");
                        ctx.phrase = phrase;
                        ctx.prediction = me._makePrediction(phrase, ctx._segmentation);
                        callback();
                    }
                });
                pending.push({index: index, request: request});
            })(k);
        }
    },

    _checkEntry: function (e, q, ctx) {
        var okey = e[0].split(" ");
        var ikey = q.ikey;
        var seg = ctx._segmentation;
        var j = (okey.length == ikey.length) ? q.end : this._matchKey(okey.slice(ikey.length), 
                                                                      q.end, 
                                                                      seg.a, 
                                                                      (q.end < seg.n) ? seg.n : -1,
                                                                      ctx.schema.ioMap);
        return j ? {unigram: e, text: e[1], start: q.start, end: j} : null;
    },

    _matchKey: function (k, i, a, predict, ioMap) {
        if (k.length == 0) {
            return i;
        }
        // predict phrase
        if (i == predict) {
            return i;
        }
        var result = 0;
        for (var j = a.length - 1; j > i; --j) {
            var kw = a[j] && a[j][i];
            if (!kw)
                continue;
            var s = ioMap[kw];
            if (s && s[k[0]]) {
                var r = this._matchKey(k.slice(1), j, a, predict, ioMap);
                if (r) {
                    result = r;
                    break;
                }
            }
        }
        return result;
    },

    _makePrediction: function (phrase, seg) {
        var total = this._dict.freqTotal + 0.1;
        var penalty = 1e-4;
        var d = [];
        for (var j = seg.m; j >= 0; --j) {
            var p = (j == seg.m) ? 1.0 : (d[j] == undefined) ? 0.0 : d[j].prob;
            for (var i = 0; i < j; ++i) {
                var c;
                if (phrase[i] && (c = phrase[i][j])) {
                    $.each(c, function (i_, e) {
                        e.prob = (e.unigram[2] + 0.1) / total * p * penalty;
                        if (d[i] == undefined || d[i].prob < e.prob) {
                            d[i] = e;
                        }
                    });
                }
            }
        }
        return d;
    },

    abortQuery: function (ctx) {
        var pending = ctx.pending;
        if (!pending || pending.length == 0)
            return;
        ctx.pending = null;
        $.each(pending, function (i_, p) {
            p.request.abort();
            //Logger.debug("aborted pending lookup: " + p.index);
        });
    },

    commit: function (ctx) {
        // TODO: save user phrase
    }

});

var GAEServerBackend = Class.extend(Backend, {

    SCHEMA_LIST: "/plume/schema_list",
    SCHEMA: "/plume/schema/",
    QUERY: "/plume/query/",
    COMMIT: "/plume/commit/",

    initialize: function () {
    },

    loadSchemaList: function (callback) {
        $.getJSON(this.SCHEMA_LIST, null, callback);
    },

    loadSchema: function (schemaName, callback) {
        $.getJSON(this.SCHEMA + encodeURIComponent(schemaName), null, function (data) {
            schema = new Schema (schemaName, data);
            callback(schema);
        });
    },

    segmentation: _segmentation,

    query: function (ctx, callback) {
        // TODO:
    },

    abortQuery: function (ctx) {
        // TODO:
    },

    commit: function (ctx) {
        // TODO:
    }

});

if (document.location.protocol == "file:") {
    Backend.register(JSONFileBackend);
}
else {
    Backend.register(GAEServerBackend);
}

var JSFrontend = Class.extend(Frontend, {
    
    initialize: function () {
        //Logger.debug("JSFrontend.initialize");
        this._backend = Backend.create();
        this._schemaList = [];
        var me = this;
        this._backend.loadSchemaList(function (schemaList) {
            me._schemaList = schemaList;
            me.onSchemaListReady(schemaList);
        });
    },

    loadSchema: function (schemaName) {
        Logger.info("loading schema: " + schemaName);
        var me = this;
        this._backend.loadSchema(schemaName, function (schema) {
            me.engine = new Engine(schema, me, me._backend);
            me.onSchemaReady();
        });
    },
    
    processKeyEvent: function (event) {
        return this.engine.processKeyEvent(event);
    }
    
    // onSchemaListReady: function (schemaList) {},
    // onSchemaReady: function () {},
    
    // updatePreedit: function (preeditText, selStart, selEnd) {},
    // updateCandidates: function (candidateList) {}, 
    // commit: function (commitText) {}, 
    // submit: function () {}
    
});

Frontend.register(JSFrontend);
