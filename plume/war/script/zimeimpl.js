// zimeimpl.js

var RomanParser = Class.extend(Parser, {

    initialize: function (schema) {
        this._alphabet = schema.getConfigCharSequence("Alphabet") || "abcdefghijklmnopqrstuvwxyz";
        this._initial = this._alphabet.split (/\s+/, 1)[0];
        this._delimiter = schema.delimiter;
        this._input = [];
    },
    
    clear: function () {
        this._input.length = 0;
    },
    
    isEmpty: function () {
        return this._input.length == 0;
    },
    
    _getInput: function() {
        // TODO: apply transform rules
        return this._input.slice(0);
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

var GroupingParser = Class.extend(Parser, {

    initialize: function (schema) {
        this._delimiter = schema.delimiter.charAt(0);
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

    _getPrompt: function (padding) {
        var keyword = this._getKeyword();
        if (padding)
            return {type: "prompt", value: this._delimiter + keyword, start: this._delimiter.length};
        else
            return {type: "prompt", value: keyword};
    },

    _commitInput: function (padding) {
        var keyword = this._getKeyword();
        this.clear();
        if (padding)
            return {type: "edit", value: [this._delimiter, keyword]};
        else
            return {type: "edit", value: [keyword]};
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
                return this._getPrompt(!ctx.isEmpty());
            }
            else {
                // keyword disposed
                return {type: "prompt", value: null};
            }
        }
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            if (this.isEmpty())
                return false;
            return this._commitInput(!ctx.isEmpty());
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
            return this._commitInput(!ctx.isEmpty());
        }
        else {
            this._cursor = k;
            return this._getPrompt(!ctx.isEmpty());
        }
    }

});

Parser.register("grouping", GroupingParser);

// TODO: ComboParser

var JSONFileBackend = Class.extend(Backend, {

    DATA_DIR: "data/",
    SCHEMA_LIST: "SchemaList.json",
    CONFIG: "Config.json",
    SEPARATOR: "/",
    JSON: ".json",

    // work around IE, which does not support ajax loading static files by encoded file name
    encode: (window.location.protocol == "file:") ? function (x) { return x; } : encodeURIComponent,

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
        $.getJSON(this.DATA_DIR + this.encode(schemaName) + this.CONFIG, null, callback);
    },

    _loadDict: function (schema, callback) {
        var me = this;
        var prefix = schema.prefix;
        $.getJSON(this.DATA_DIR + this.encode(prefix) + this.JSON, null, function (data) {
            data.prefix = prefix;
            me._dict = data;
            if (callback) {
                callback(schema);
            }
        });
    },

    segmentation: function (schema, input) {
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
                if (!schema.keywords[s]) {
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
                a[t][i] = schema.keywords[s];
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
    },

    query: function (ctx, callback) {
        if (!this._dict) {
            Logger.error("dict not yet loaded.");
            return;
        }
        this._indexQueries(ctx);
        this._lookup(ctx, callback);
    },

    _indexQueries: function (ctx) {
        var maxKeywordLength = ctx.schema.maxKeywordLength;
        var maxKeyLength = ctx.schema.maxKeyLength;
        var a = ctx._segmentation.a;
        var b = ctx._segmentation.b;
        this._queries = {};
        var q = [];
        for (var i = 0; i < b.length; ++i) {
            var s = [];
            for (var j = i + 1; j < b.length && b[j] - b[i] <= maxKeywordLength; ++j) {
                var kw = a[b[j]][b[i]];
                if (kw) {
                    var r = {ikey: [kw], start: b[i], end: b[j]};
                    this._registerQuery(r);
                    if (r.ikey.length < maxKeyLength)
                        s.push(r);
                    var me = this;
                    $.each(q, function (k, e) {
                        if (e.end == b[i]) {
                            r = {ikey: e.ikey.concat([kw]), start: e.start, end: b[j]};
                            me._registerQuery(r);
                            if (r.ikey.length < maxKeyLength)
                                s.push(r);
                        }
                    });
                }
            }
            q = q.concat(s);
        }
    },

    _registerQuery: function (r) {
        //Logger.debug("registerQuery: [" + r.start + ", " + r.end + ") " + r.ikey);
        var index = r.ikey.slice(0, this._dict.indexingLevel).join("_");
        if (this._queries[index])
            this._queries[index].push(r);
        else
            this._queries[index] = [r];
    },

    _lookup: function (ctx, callback) {
        var prefix = this._dict.prefix;
        var queries = this._queries;
        var me = this;
        var pending = [];
        var phrase = [];
        for (var k in queries) {
            (function (index) {
                //Logger.debug("pending: " + index);
                pending.push(index);
                $.getJSON(me.DATA_DIR + me.encode(prefix) + me.SEPARATOR + me.encode(index) + me.JSON, null, function (data) {
                    Logger.debug("fetched: " + index);
                    if (me._queries !== queries)
                        return;
                    // generate query results
                    $.each(queries[index], function (i, q) {
                        var p = phrase[q.start];
                        if (!p) {
                            p = phrase[q.start] = [];
                        }
                        // filter results
                        $.each(data[q.ikey.join(" ")] || [], function (i, e) {
                            var r = me._checkEntry(e, q, ctx);
                            if (r) {
                                if (!p[r.end])
                                    p[r.end] = [r];
                                else
                                    p[r.end].push(r);
                            }
                        });
                    });
                    var pos = -1;
                    $.each(pending, function (i, e) {
                        if (e == index) {
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
                        me._queries = null;
                        Logger.debug("lookup successful.");
                        ctx.phrase = phrase;
                        ctx.prediction = me._makePrediction(phrase, ctx._segmentation);
                        callback();
                    }
                });
            })(k);
        }
    },

    _checkEntry: function (e, q, ctx) {
        var okey = e[0].split(" ");
        var ikey = q.ikey;
        var j = (okey.length == ikey.length) ? q.end : this._matchKey(okey.slice(ikey.length), 
                                                                      q.end, 
                                                                      ctx._segmentation, 
                                                                      ctx.schema.fuzzyMap);
        return j ? {unigram: e, text: e[1], start: q.start, end: j} : null;
    },

    _matchKey: function (k, i, seg, fuzzyMap) {
        if (k.length == 0) {
            Logger.debug("matchKey: " + i);
            return i;
        }
        // predict phrase
        if (i == seg.n) {
            return i;
        }
        var result = 0;
        var me = this;
        $.each(seg.a, function (j, c) {
            if (c && c[i]) {
                var found = false;
                $.each(fuzzyMap[c[i]], function (i_, t) {
                    if (k[0] == t) {
                        found = true;
                        return false;
                    }
                });
                if (found) {
                    var r = me._matchKey(k.slice(1), j, seg, fuzzyMap);
                    if (r) {
                        result = r;
                        return false;
                    }
                }
            }
        });
        return result;
    },

    _makePrediction: function (phrase, seg) {
        var total = this._dict.freqTotal + 0.1;
        var d = [];
        for (var j = seg.m; j >= 0; --j) {
            var p = (j == seg.m) ? 1.0 : (d[j] == undefined) ? 0.0 : d[j].prob;
            for (var i = 0; i < j; ++i) {
                var c;
                if (phrase[i] && (c = phrase[i][j])) {
                    $.each(c, function (i_, e) {
                        e.prob = (e.unigram[2] + 0.1) / total * p;
                        if (d[i] == undefined || d[i].prob < e.prob) {
                            d[i] = e;
                        }
                    });
                }
            }
        }
        return d;
    }

});

Backend.register(JSONFileBackend);

var JSFrontend = Class.extend(Frontend, {
    
    initialize: function () {
        Logger.debug("JSFrontend.initialize");
        this._backend = Backend.create();
        this._schemaList = [];
        var me = this;
        this._backend.loadSchemaList(function (schemaList) {
            me._schemaList = schemaList;
            me.onSchemaListReady(schemaList);
            if (schemaList.length > 0) {
                me.loadSchema(schemaList[0].schema);
            }
        });
    },
    
    loadSchema: function (schemaName) {
        Logger.debug("JSFrontend.loadSchema: " + schemaName);
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
