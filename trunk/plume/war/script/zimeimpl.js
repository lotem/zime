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
    
    getPrompt: function () {
        return null;
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

var JSONFileBackend = Class.extend(Backend, {

    DATA_DIR: "data/",
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
        $.getJSON(this.DATA_DIR + schemaName + this.CONFIG, null, callback);
    },

    _loadDict: function (schema, callback) {
        var me = this;
        var prefix = schema.prefix;
        $.getJSON(this.DATA_DIR + prefix + this.JSON, null, function (data) {
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
                // TODO: return true if suggested by DivideRule
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

    _dummySegmentation: function (schema, input) {
        var n = input.length;
        var a = [];
        for (var i = 0; i <= n; ++i) {
            a[i] = [];
        }
        var b = [0];
        var d = [0];
        var delim = schema.delimiter;
        var i = 0;
        while (i < n) {
            var j = i;
            while (j < n && j - i < schema.maxKeywordLength && delim.indexOf(input[j]) == -1)
                ++j;
            var spelling = input.slice(i, j).join("");
            var ikey = schema.keywords[spelling];
            Logger.debug("spelling: " + spelling);
            if (ikey == undefined)
                break;
            if (j < n && delim.indexOf(input[j]) != -1) {
                j += 1;
                d.push(j);
            }
            b.push(j);
            a[j][i] = ikey;
            Logger.debug("a[" + j + "][" + i + "]: " + ikey);
            i = j;
        }
        if (b[b.length - 1] > d[d.length - 1]) {
            d.push(b[b.length - 1]);
        }
        var m = i;
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
                $.getJSON(me.DATA_DIR + prefix + me.SEPARATOR + index + me.JSON, null, function (data) {
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
                            var r = me._checkEntry(e, ctx._segmentation, q.start, q.end);
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

    _checkEntry: function (e, seg, i, j) {
        // TODO: match okey to ikey
        return {unigram: e, text: e[1], start: i, end: j};
    },

    _makePrediction: function (phrase, seg) {
        // TODO
        return [];
    },

    _dummyQuery: function (ctx, callback) {
        var seg = ctx._segmentation;
        var b = seg.b;
        var c = [];
        var d = [];
        for (var i = 0; i < b.length; ++i) {
            c[b[i]] = [];
            for (var j = i + 1; j <= b.length && b[j] <= b[i] + ctx.schema.maxKeywordLength; ++j) {
                var t = seg.a[b[j]][b[i]];
                if (t) {
                    Logger.log("[" + b[i] + ", " + b[j] + ") " + t);
                    var cl = [];
                    for (var k = 1; k <= 9; ++k) {
                        cl.push({
                            text: t.toUpperCase() + k, 
                            start: b[i], 
                            end: b[j], 
                            prob: 0.01
                        });
                    }
                    c[b[i]][b[j]] = cl;
                    d[b[i]] = cl[0];
                }
            }
        }
        ctx.phrase = c;
        ctx.prediction = d;
        callback();
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
