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
    var parserName = schema.getParserName();
    var klass = this._registry[parserName];
    if (klass == undefined)
        return null;
    Logger.info("creating parser: " + parserName);
    return new klass();
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
        this.parser = this.getConfigValue("Parser");
        this.maxKeyLength = this.getConfigValue("MaxKeyLength") || 3;
        this.maxKeywordLength = this.getConfigValue("MaxKeywordLength") || 7;
        this.delimiter = this.getConfigCharSequence("Delimiter") || " ";
        this.alphabet = this.getConfigCharSequence("Alphabet") || "abcdefghijklmnopqrstuvwxyz";
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

    getParserName: function () {
        return this.getConfigValue("Parser");
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
    // TODO
});

var Engine = new Class({

    initialize: function (schema, frontend, backend) {
        Logger.debug("Engine.initialize");
        this.schema = schema;
        this._frontend = frontend;
        this._backend = backend;
        this._ctx = new Context(schema);
        this.updateUI();
    },
    
    updateUI: function () {
    	// TODO: test code
    	this._frontend.updatePreedit('abc', 1, 3);
    	this._frontend.updateCandidates(['A', 'B', 'C']);
    },
    
    processKeyEvent: function (event) {
    	// TODO: test code
    	if (event.type == "keyup") {
	    	this._frontend.commit('keyup!');
	    	this.updateUI();
    	}
    	return true;
    }

});

