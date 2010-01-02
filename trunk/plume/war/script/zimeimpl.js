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
    JSON: ".json",

    loadSchemaList: function (callback) {
        $.getJSON(this.DATA_DIR + this.SCHEMA_LIST, null, callback);
    },

    loadConfig: function (schemaName, callback) {
        $.getJSON(this.DATA_DIR + schemaName + this.CONFIG, null, callback);
    },

    loadDict: function (schema) {
        var prefix = schema.prefix;
        $.getJSON(this.DATA_DIR + prefix + this.JSON, null, function (data) {
            this._dict = data;
            this._dict.prefix = prefix;
        });
    },

    segmentation: function (schema, input) {
        // TODO: 
        return this._dummySegmentation(schema, input);
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
        // TODO: 
        this._dummyQuery(ctx, callback); 
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
        this._backend.loadConfig(schemaName, function (config) {
            schema = new Schema (schemaName, config);
            me._backend.loadDict(schema);
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
