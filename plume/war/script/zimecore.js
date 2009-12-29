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
        		value = ["alt", a.slice(1)];
        	} else if (a.length == 2) {
        		var b = a[1];
        		if (b.indexOf("~") != -1) {
        			value = ["pair", b.split("~", 2)];
        		} else {
        			value = ["unique", b];
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
		this.updateUI = function () {
			engine.onContextUpdate(this);
		}
		this.updateUI();
	},
	
    // TODO
    beingConverted: function () {
		return false;
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
    	// TODO: test code
    	this._frontend.updatePreedit('abc', 1, 3);
    	this._frontend.updateCandidates(['A', 'B', 'C']);
    },
    
    processKeyEvent: function (event) {
    	// TODO: test code
    	if (event.type == "keyup") {
	    	this._frontend.commit('keyup!');
	    	this.ctx.updateUI();
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
    	table[keyCode] = [lowerCase[i], upperCase[i]];
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
