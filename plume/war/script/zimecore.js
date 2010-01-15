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
    // isEmpty: function () {},
    // processInput: function (event, ctx) {}
});

Parser._registry = {};

Parser.register = function (parserName, klass) {
    this._registry[parserName] = klass;
    Logger.info("registered parser: " + parserName);
};

Parser.create = function (schema) {
    var parserName = schema.parser;
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
    // submit: function () {},
    // loadSchema: function (schemaName) {},
    // processKeyEvent: function (event) {}
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
    // loadSchema: function (schemaName, callback) {},
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
        $.extend(this, data);
        // required configuration options
        this.dict = this.getConfigValue("Dict");
        this.parser = this.getConfigValue("Parser");
        this.maxKeyLength = Math.max(2, Number(this.getConfigValue("MaxKeyLength") || 2));
        this.maxKeywordLength = Number(this.getConfigValue("MaxKeywordLength") || 7);
        this.delimiter = this.getConfigCharSequence("Delimiter") || " ";
        this.alphabet = this.getConfigCharSequence("Alphabet") || "abcdefghijklmnopqrstuvwxyz";
        this.initial = this.alphabet.split (/\s+/, 1)[0];
        var xformRules = $.map(this.getConfigList("TransformRule"), function (r) {
            var p = r.split(/\s+/);
            return {pattern: new RegExp(p[0], "g"), repl: p[1]};
        });
        this.xform = (xformRules.length == 0) ? null : function (s) {
            // apply transform rules
            $.each(xformRules, function (i_, r) {
                s = s.replace(r.pattern, r.repl);
            });
            return s;
        };
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
        var m = $.grep(this.config, function (e) {
            return e[0] == key;
        });
        if (m.length != 1)
            return null;
        return m[0][1];
    },

    getConfigList: function (key) {
        var m = $.grep(this.config, function (e) {
            return e[0] == key;
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

    PAGE_SIZE: 5,

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
        this._display = null;
        this._selected = [];
        this._current = null;
        this._candidateList = null;
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
        return this._current != null;
    },
    
    isCompleted: function () {
        return this._current && this._current.end == this.input.length;
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
            this._display = this._calculateDisplayText(input, seg);
            if (seg.m != seg.n) {
                this._error = {start: seg.m, end: seg.n};
            }
        }
        this._updateUI();
    },
    
    convert: function () {
        //Logger.debug("convert:");
        if (this._error) {
            return false;
        }
        var ctx = this;
        this._backend.query(this, function () {
            ctx._updateCandidates(ctx._predict());
        });
        return true;
    },
    
    cancelConversion: function () {
        //Logger.debug("cancelConversion:");
        this.edit();
        return true;
    },
    
    back: function () {
        if (!this.beingConverted() || this._selected.length == 0) {
            return false;
        }
        var last = this._selected.pop();
        //Logger.debug("back: " + last.text);
        this._updateCandidates(last.start);
        return true;
    },
    
    forth: function () {
        if (!this.beingConverted()) {
            return false;
        }
        var p = this.prediction[this._current.start];
        if (p && p.end < this._segmentation.m) {
            this._selected.push(p);
            var i = p.end;
            var c = this.phrase[i];
            var m = this._segmentation.m;
            var j = 0;
            for (var k = i + 1; k <= m; ++k) {
                if (c[k]) {
                    j = k;
                    break;
                }
            }
            this._updateCandidates(i, j);
            return true;
        }
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
        var i = this._current.start;
        var c = this.phrase[i];
        for (var j = this._current.end - 1; j > i; --j) {
            if (c[j]) {
                this._updateCandidates(i, j);
                return true;
            }
        }
        this.back();
        return true;
    },

    right: function () {
        if (!this.beingConverted()) {
            return false;
        }
        var i = this._current.start;
        var c = this.phrase[i];
        var m = this._segmentation.m;
        for (var j = this._current.end + 1; j <= m; ++j) {
            if (c[j]) {
                this._updateCandidates(i, j);
                return true;
            }
        }
        this.forth();
        return true;
    },

    _updateCandidates: function (i, j) {
        //Logger.debug("_updateCandidates: " + i + ", " + j);
        if (!j) {
            j = this._segmentation.m;
        }
        var result = [];
        var c = this.phrase[i];
        if (c) {
            for (var k = j; k > i; --k) {
                if (c[k]) {
                    result = result.concat(c[k]);
                }
            }
        }
        if (result.length > 0) {
            result.currentPage = 0;
            this._candidateList = result;
            this._current = result[0];
        }
        else {
            this._candidateList = null;
            this._current = null;
            this._error = {start: i, end: j};
        }
        this._updateUI();
    },

    _predict: function () {
        var s = this._selected;
        var i = (s.length > 0) ? s[s.length - 1].end : 0;
        var p;
        while ((p = this.prediction[i]) != undefined) {
            if (p.end >= this._segmentation.m)
                break;
            this._selected.push(p);
            i = p.end;
        }
        return i;
    },

    select: function (choice) {
        if (choice >= this.PAGE_SIZE) {
            return false;
        }
        var c = this._candidateList;
        var index = c.currentPage * this.PAGE_SIZE + choice;
        if (index >= c.length) {
            return false;
        }    
        this._current = c[index];
        return true;
    },

    forward: function () {
        this._selected.push(this._current);
        this._updateCandidates(this._current.end);
    },

    pageUp: function () {
        var c = this._candidateList;
        if (!c || c.currentPage == 0) {
            return false;
        }
        --c.currentPage;
        this._current = c[c.currentPage * this.PAGE_SIZE];
        this._updateUI();
        return true;
    },

    pageDown: function () {
        var c = this._candidateList;
        if (!c || (c.currentPage + 1) * this.PAGE_SIZE >= c.length) {
            return false;
        }
        ++c.currentPage;
        this._current = c[c.currentPage * this.PAGE_SIZE];
        this._updateUI();
        return true;
    },
    
    commit: function () {
        this._selected.push(this._current);
        var commitText = $.map(this._selected, function (e) {
            return e.text;
        }).join("");
        // TODO: save user phrase
        this.clear();
        return commitText;
    },

    _calculateDisplayText: function (input, seg) {
        if (input.length == 0)
            return null;
        var index = [];
        var disp = [];
        var delimiter = this.schema.delimiter;
        var j = 0;
        var k = 0;
        for (var i = 0; i < seg.n; ++i) {
            if (i <= seg.m && i == seg.d[k]) {
                ++k;
                if (i > 0 && delimiter.indexOf(input[i - 1]) == -1) {
                    disp.push(delimiter.charAt(0));
                    j += 1;
                }
            }
            index[i] = j;
            disp.push(input[i]);
            j += input[i].length;
        }
        index[seg.n] = j;
        return {text: disp.join(""), index: index};
    },

    getPreedit: function () {
        var result = [];
        var start = 0;
        var end = 0;
        var rest = 0;
        if (this._current) {
            $.each(this._selected, function (i, e) {
                result.push(e.text);
                start += e.text.length;
            });
            result.push(this._current.text);
            end = start + this._current.text.length;
            rest = this._current.end;
        }
        if (rest < this.input.length) {
            var text = this._display.text;
            var idx = this._display.index;
            result.push(text.substring(idx[rest]));
            if (this._error) {
                var diff = idx[rest] - end;
                start = idx[this._error.start] - diff;
                end = idx[this._error.end] - diff;
            }
        }
        return {
            text: result.join(""),
            start: start,
            end: end
        };
    },

    // returns an array of strings
    getCandidates: function () {
        var c = this._candidateList;
        if (!c) {
            return [];
        }
        var start = this.PAGE_SIZE * c.currentPage;
        var end = Math.min(start + this.PAGE_SIZE, c.length);
        return $.map(c.slice(start, end), function (e) {
            return e.text;
        });
    }

});

var Engine = new Class({

    initialize: function (schema, frontend, backend) {
        //Logger.debug("Engine.initialize");
        this.schema = schema;
        this._frontend = frontend;
        this._parser = Parser.create(schema);
        this.ctx = new Context(schema, this, backend);
    },
    
    onContextUpdate: function (ctx) {
        //Logger.debug("onContextUpdate: " + ctx.input);
        var p = ctx.getPreedit();
        this._frontend.updatePreedit(p.text, p.start, p.end);
        this._frontend.updateCandidates(ctx.getCandidates());
    },
    
    processKeyEvent: function (event) {
        // handle hot keys  --zouive
        if (event.altKey || event.metaKey) {
            return false;
        }
        if (event.ctrlKey) {
            if (this.ctx.isEmpty() && event.type == "keydown") {
                if (event.keyCode == KeyEvent.KEY_ENTER) {
                    //Logger.debug("processKeyEvent: submit triggered (Ctrl+Enter)");
                    this._frontend.submit();            
                    return true;
                }
                else if (event.keyCode == KeyEvent.KEY_A + 2) {
                    //Logger.debug("processKeyEvent: submit triggered (Ctrl+C)");
                    this._frontend.submit();            
                    return false;
                }
            }
            return false;
        }
        //  --zouive
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
            return true;
        }
        if (result.type == "prompt") {
            var p = this.ctx.getPreedit();
            if (result.value == null) {
                this._frontend.updatePreedit(p.text, p.start, p.end);
            }
            else {
                var text = p.text + result.value;
                var start = p.text.length + (result.start || 0);
                var end = p.text.length + (result.end || result.value.length);
                this._frontend.updatePreedit(text, start, end);
            }
            return true;
        }
        if (result.type == "keyevent") {
            return this._process(result.value);
        }
        // noop
        return true;
    },
    
    _forward: function () {
        if (this.ctx.isCompleted()) {
            this._commit();
            this._parser.clear();
        }
        else {
            this.ctx.forward();
        }
    },

    _commit: function () {
        var s = this.ctx.commit();
        this._frontend.commit(s);
    },

    _commitRawInput: function () {
        var s = this.ctx.input.join("");
        this.ctx.clear();
        this._frontend.commit(s);
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
                this._parser.clear();
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
            // commit printable chars directly to the editor
            if (event.type == "keydown") {
                // clear prompt
                this._frontend.updatePreedit("", 0, 0);
                if (event.keyCode == KeyEvent.KEY_ENTER) {
                    this._frontend.commit("\n");
                    return true;
                }
                var ch = KeyEvent.toChar(event);
                if (ch) {
                    this._frontend.commit(ch);
                    return true;
                }
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
                ctx.select(0) && this._forward();
            } else {
                ctx.convert();
            }
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_ENTER) {
            if (ctx.beingConverted()) {
                ctx.select(0) && this._forward();
            } else {
                this._commitRawInput();
                this._parser.clear();
            }
            return true;
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
        if (event.keyCode == KeyEvent.KEY_PAGEUP || event.keyCode == KeyEvent.KEY_UP) {
            if (ctx.beingConverted()) {
                ctx.pageUp();
            }
            return true;
        }
        if (event.keyCode == KeyEvent.KEY_PAGEDOWN || event.keyCode == KeyEvent.KEY_DOWN) {
            if (ctx.beingConverted()) {
                ctx.pageDown();
            }
            return true;
        }
        var ch = KeyEvent.toChar(event);
        if (ctx.beingConverted()) {
            if (ch == "-" || ch == ",") {
                ctx.pageUp();
                return true;
            }
            if (ch == "=" || ch == ".") {
                ctx.pageDown();
                return true;
            }
            if (ch >= "1" && ch <= "9") {
                if (ctx.select(ch - "1")) {
                    this._forward();
                    return true;
                }
                // try matching punctuation
            }
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
