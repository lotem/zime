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
    
    _isInput: function (ch) {
        if (this.isEmpty()) {
            return this._initial.indexOf(ch) != -1;
        } else {
            return this._alphabet.indexOf(ch) != -1 || this._delimiter.indexOf(ch) != -1;
        }
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
            this._input.pop();
            ctx.input = this._getInput();
            return {type: "edit", value: null};
        }
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            return false;
        }
        var ch = KeyEvent.toChar(event);
        if (ch != null && this._isInput(ch)) {
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

    loadSchemaList: function (callback) {
        $.getJSON(this.DATA_DIR + this.SCHEMA_LIST, null, callback);
    },

    loadConfig: function (schemaName, callback) {
        $.getJSON(this.DATA_DIR + schemaName + this.CONFIG, null, callback);
    },

    segmentation: function (schema, input) {
        // TODO: dummy code
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

    query: function (ctx, callback) {}
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
