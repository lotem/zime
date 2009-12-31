// zimecore.js

var Class = function (proto) {
    var klass = function () {
        if (typeof this.initialize == "function") {
            this.initialize.apply(this, arguments);
        }
    };
    if (proto != undefined) {
        klass.prototype = proto;
        klass.prototype.constructor = klass;
    }
    return klass;
};

Class.extend = function (klass, extension) {
    return new Class($.extend({}, klass.prototype, extension));
};

var Logger = {};
var console;

Logger.log = function (text) {
    if (console != undefined) {
        console.log(text);
    }
};

Logger.debug = function (text) {
    if (console != undefined) {
        console.debug("[DEBUG] " + text);
    }
};

Logger.info = function (text) {
    if (console != undefined) {
        console.info("[INFO] " + text);
    }
};

Logger.error = function (text) {
    if (console != undefined) {
        console.error("[ERROR] " + text);
    }
};

// abstarct class
var Parser = new Class({
    // clear: function () {},
    // getPrompt: function () {},
    // processInput: function (event, ctx) {}
});

Parser._registry = {};

Parser.register = function (parserName, klass) {
    this._registry[parserName] = klass;
    Logger.info("registered parser: " + parserName);
};

Parser.create = function (schema) {
    var parserName = schema.getConfigValue("Parser");
    var klass = this._registry[parserName];
    if (klass == undefined)
        return null;
    Logger.info("creating parser: " + parserName);
    return new klass(schema);
};

// abstract class
var Frontend = new Class({
    // updatePreedit: function (preeditText, selStart, selEnd) {},
    // updateCandidates: function (candidateList) {}, 
    // commit: function (commitText) {}, 
    // submit: function () {}
});

Frontend.register = function (klass) {
    this._impl = klass;
    Logger.info("registered frontend");
}

Frontend.create = function () {
    Logger.info("creating frontend");
    return new this._impl();
}

// abstract class
var Backend = new Class({
    // loadSchemaList: function (callback) {},
    // loadConfig: function (schemaName, callback) {},
    // query: function (input, callback) {}
});

Backend.register = function (klass) {
    this._impl = klass;
    Logger.info("registered backend");
}

Backend.create = function () {
    Logger.info("creating backend");
    return new this._impl();
}

var Schema = new Class({

    initialize: function (schemaName, data) {
        this.schemaName = schemaName;
        this._prefix = "Config/" + schemaName + "/";
        $.extend(this, data);
        // required configuration options
        this.maxKeyLength = this.getConfigValue("MaxKeyLength") || 3;
        this.maxKeywordLength = this.getConfigValue("MaxKeywordLength") || 7;
        this.delimiter = this.getConfigCharSequence("Delimiter") || " ";
        var punct = {};
        $.each(this.getConfigList("Punct"), function (i, p) {
            var a = p.split(/\s+/);
            var key = a[0];
            var value;
            if (a.length > 2) {
                value = {type: "alt", value: a.slice(1)};
            } else if (a.length == 2) {
                var b = a[1];
                if (b.indexOf("~") != -1) {
                    value = {type: "pair", value: b.split("~", 2)};
                } else {
                    value = {type: "unique", value: b};
                }
            }
            punct[key] = value;
        });
        this.punct = punct;
    },

    getConfigValue: function (key) {
        var path = this._prefix + key;
        var m = $.grep(this.config, function (e) {
            return e[0] == path;
        });
        if (m.length != 1)
            return null;
        return m[0][1];
    },

    getConfigList: function (key) {
        var path = this._prefix + key;
        var m = $.grep(this.config, function (e) {
            return e[0] == path;
        });
        return $.map(m, function (e) {
            return e[1];
        });
    },

    getConfigCharSequence: function (key) {
        var s = this.getConfigValue(key);
        var re = /^\[.*\]$/;
        if (s != null && s.match(re))
            return s.slice(1, -1);
        else
            return s;
    }

});

var Context = new Class({

    initialize: function (schema, engine, backend) {
        this.schema = schema;
        this._backend = backend;
        this._updateUI = function () {
            engine.onContextUpdate(this);
        }
        this._reset();
        this._updateUI();
    },
    
    _reset: function () {
        this.input = [];
        this._error = null;
        this._segmentation = null;
        this._selected = [];
        this._current = [];
    },

    clear: function () {
        this._reset();
        this._updateUI();
    },

    isEmpty: function () {
        return this.input.length == 0;
    },
    
    popInput: function () {
        this.input.pop();
        if (!this.isEmpty() && this.input[this.input.length - 1] == this.schema.delimiter[0]) {
            this.input.pop();
        }
    },
    
    beingConverted: function () {
        return this._current.length != 0;
    },
    
    isCompleted: function () {
        var c = this._current;
        return c.length > 0 && c[c.length - 1].end == this.input.length;
    },
    
    edit: function (input) {
        if (input == undefined) {
            input = this.input;
        }
        this._reset();
        this.input = input;
        if (input.length > 0) {
            var seg = this._backend.segmentation (this.schema, input);
            this._segmentation = seg;
            if (seg.m != seg.n) {
                this._error = {start: seg.m, end: seg.n};
            }
        }
        this._updateUI();
    },
    
    convert: function () {
        Logger.debug("convert:");
        // TODO:
        // this._backend.query(this, ...);
        this._updateCandidates(this._predict());
        return true;
    },
    
    cancelConversion: function () {
        this.edit();
        return true;
    },
    
    back: function () {
        if (!this.beingConverted()) {
            return false;
        }
        // TODO
        return false;
    },
    
    forth: function () {
        if (!this.beingConverted()) {
            return false;
        }
        // TODO
        return false;
    },

    home: function () {
        if (!this.beingConverted()) {
            return false;
        }
        this._selected = [];
        this._updateCandidates(0);
        return true;
    },

    end: function () {
        if (!this.beingConverted()) {
            return false;
        }
        this._selected = [];
        this._updateCandidates(this._predict());
    },

    left: function () {
        if (!this.beingConverted()) {
            return false;
        }
        // TODO:
        return true;
    },

    right: function () {
        if (!this.beingConverted()) {
            return false;
        }
        // TODO:
        return true;
    },

    // TODO
    _updateCandidates: function () {
    },

    _predict: function () {
    },

    select: function (choice) {
        // TODO
        return false;
    },

    pageUp: function () {
    },

    pageDown: function () {
    },
    
    commit: function () {
        // TODO: save user phrase
        this.clear();
    },

    getCommitText: function () {
        return "TODO";
    },

    getPreedit: function () {
        // TODO
        var start = 0;
        var end = 0;
        if (this._error) {
            start = this._error.start;
            end = this._error.end;
        }
        return {
            text: this.input.join(""),
            start: start,
            end: end
        };
    },

    getCandidates: function () {
        // TODO
        return [];
    }

});

var Engine = new Class({

    initialize: function (schema, frontend, backend) {
        Logger.debug("Engine.initialize");
        this.schema = schema;
        this._frontend = frontend;
        this._parser = Parser.create(schema);
        this.ctx = new Context(schema, this, backend);
    },
    
    onContextUpdate: function (ctx) {
        Logger.debug("onContextUpdate: " + ctx.input);
        var p = ctx.getPreedit();
        this._frontend.updatePreedit(p.text, p.start, p.end);
        this._frontend.updateCandidates(ctx.getCandidates());
    },
    
    processKeyEvent: function (event) {
        // ignore hot keys
        if (event.ctrlKey || event.altKey || event.metaKey) {
            return false;
        }
        // handle alternating punctuation input
        if (this._punct && event.type != "keyup" && event.keyCode != KeyEvent.KEY_SHIFT) {
            var ch = KeyEvent.toChar(event);
            if (ch == this._punctKey) {
                this._alternatePunct();
               return true;
            } else {
                if (event.keyCode == KeyEvent.KEY_BACKSPACE || event.keyCode == KeyEvent.KEY_ESCAPE) {
                    this._clearPunctPrompt();
                    return true;
                }
                var p = this._punct;
                this._frontend.commit(p.value[p.lastIndex]);
                this._clearPunctPrompt();
                if (event.keyCode == KeyEvent.KEY_SPACE || event.keyCode == KeyEvent.KEY_ENTER) {
                    return true;
                }
            }
        }
        var result = this._parser.processInput(event, this.ctx);
        if (typeof result == "boolean") {
            if (result)
                return true;
            else
                return this._process(event);
        }
        if (result.type == "edit") {
            if (this.ctx.isCompleted()) {
                this._commit();
                // continue processing
            } else if (this.ctx.beingConverted()) { 
                return true;
            }
            // input updated
            if (result.value == null)
                this.ctx.edit();
            else
                this.ctx.edit(this.ctx.input.concat(result.value));
        }
        // TODO: handle other result types
        // NOOP
        return true;
    },
    
    _commit: function () {
        var s = ctx.getCommitText();
        this._frontend.commit(s);
        this._parser.clear();
        this.ctx.commit();
    },
    
    _handlePunct: function (event, autoCommit) {
        var ch = KeyEvent.toChar(event);
        var p = this.schema.punct[ch];
        if (p != undefined) {
            if (event.type == "keyup") {
                return true;
            }
            if (autoCommit) {
                this._commit();
            }
            if (p.type == "unique") {
                this._frontend.commit(p.value);
            } else if (p.type == "pair") {
                var idx = p.lastIndex;
                idx = (idx == undefined) ? 0 : 1 - idx;
                p.lastIndex = idx;
                this._frontend.commit(p.value[idx]);
            } else if (p.type == "alt") {
                this._punct = p;
                this._punctKey = ch;
                p.lastIndex = 0;
                var symbol = p.value[0];
                this._frontend.updatePreedit(symbol, 0, symbol.length);
            }
            return true;
        }
        return false;
    },

    _alternatePunct: function () {
        var p = this._punct;
        var idx = (p.lastIndex + 1) % p.value.length;
        p.lastIndex = idx;
        var symbol = p.value[idx];
        this._frontend.updatePreedit(symbol, 0, symbol.length);
    },

    _clearPunctPrompt: function () {
        this._punct = undefined;
        this._punctKey = undefined;
        this._frontend.updatePreedit("", 0, 0);
    },
    
    _process: function (event) {
        var ctx = this.ctx;
        if (ctx.isEmpty()) {
            if (event.keyCode == KeyEvent.KEY_TAB) {
                return false;
            }
            if (this._punct && event.type == "keyup") {
                return true;
            }
            if (this._handlePunct(event, false)) {
                return true;
            }
            return true;
        }
        if (event.type == "keyup") {
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_ESCAPE) {
            if (ctx.beingConverted()) {
                ctx.cancelConversion();
            } else {
                ctx.edit([]);
            }
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_BACKSPACE) {
            if (ctx.beingConverted()) {
                ctx.back() || ctx.cancelConversion();
            } else {
                ctx.popInput();
                ctx.edit();
            }
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            if (ctx.beingConverted()) {
                ctx.select(0);
            } else {
                ctx.convert();
            }
        }
        if (event.keyCode == KeyEvent.KEY_ENTER) {
            // TODO: handle Shift+Enter
            if (ctx.beingConverted()) {
                ctx.select(0);
            } else {
                this._commit();
            }
        }
        if (event.keyCode == KeyEvent.KEY_TAB) {
            ctx.convert();
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_HOME) {
            ctx.home();
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_END) {
            ctx.end();
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_LEFT) {
            ctx.left();
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_RIGHT) {
            ctx.right();
            return true;
        }
        var hasCandidates = ctx.getCandidates().length > 0;
        if (event.keyCode == KeyEvent.KEY_PAGEUP || event.keyCode == KeyEvent.KEY_UP) {
            if (hasCandidates) {
                ctx.pageUp();
            }
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_PAGEDOWN || event.keyCode == KeyEvent.KEY_DOWN) {
            if (hasCandidates) {
                ctx.pageDown();
            }
            return true;
        }
        var ch = KeyEvent.toChar(event);
        if (hasCandidates) {
            if (ch == "-" || ch == ",") {
                ctx.pageUp();
                return true;
            }
            if (ch == "=" || ch == ".") {
                ctx.pageDown();
                return true;
            }
        }
        if (ch >= "1" && ch <= "9") {
            if (ctx.select(ch - "1")) {
                return true;
            }
            // try matching punctuation
        }
        // auto-commit
        if (this.ctx.isCompleted() && this._handlePunct(event, true)) {
            return true;
        }
        return true;
    }

});

KeyEvent = {
    KEY_SHIFT: 16,
    KEY_CTRL: 17,
    KEY_ALT: 18,
    KEY_LEFT: 37,
    KEY_RIGHT: 39,
    KEY_UP: 38,
    KEY_DOWN: 40,
    KEY_HOME: 36,
    KEY_END: 35,
    KEY_PAGEDOWN: 34,
    KEY_PAGEUP: 33,
    KEY_DELETE: 46,
    KEY_BACKSPACE: 8,
    KEY_ESCAPE: 27,
    KEY_TAB: 9,
    KEY_ENTER: 13,
    KEY_SPACE: 32,
    KEY_0: 48,
    KEY_1: 49,
    KEY_9: 57,
    KEY_A: 65,
    KEY_Z: 90,
    KEY_BACKQUOTE: 192,
    KEY_MINUS_1: 109,
    KEY_MINUS_2: 189,
    KEY_EQUAL_1: 107,
    KEY_EQUAL_2: 187,
    KEY_L_BRACKET: 219,
    KEY_R_BRACKET: 221,
    KEY_BACKSLASH: 220,
    KEY_SEMICOLON_1: 59,
    KEY_SEMICOLON_2: 186,
    KEY_APOSTROPHE: 222,
    KEY_COMMA: 188,
    KEY_PERIOD: 190,
    KEY_SLASH: 191
};

(function () {
    var keyCodes = [
        192, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 109, 189, 107, 187,  
        81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 219, 221, 220, 
        65, 83, 68, 70, 71, 72, 74, 75, 76, 59, 186, 222,
        90, 88, 67, 86, 66, 78, 77, 188, 190, 191,
        32
    ];
    var lowerCase = "`1234567890--==qwertyuiop[]\\asdfghjkl;;\'zxcvbnm,./ ";
    var upperCase = "~!@#$%^&*()__++QWERTYUIOP{}|ASDFGHJKL::\"ZXCVBNM<>? ";
    var table = {};
    $.each(keyCodes, function (i, keyCode) {
        table[keyCode] = [lowerCase.charAt(i), upperCase.charAt(i)];
    });
    KeyEvent._table = table;
})();

KeyEvent.toChar = function (event) {
    if (event.ctrlKey || event.altKey || event.metaKey)
        return null;
    var keyCode = event.keyCode;
    if (keyCode < 0 || keyCode >= 256 || !this._table[keyCode])
        return null;
    return this._table[keyCode][event.shiftKey ? 1 : 0];
};
