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

Logger.log = function (text) {
    if (console != undefined)
        console.log(text);
};

Logger.debug = function (text) {
    if (console != undefined)
        console.debug("[DEBUG] " + text);
};

Logger.info = function (text) {
    if (console != undefined)
        console.info("[INFO] " + text);
};

Logger.error = function (text) {
    if (console != undefined)
        console.error("[ERROR] " + text);
};

// abstarct class
var Parser = Class({
    // clear: function () {},
    // getPrompt: function () {},
    // processInput: function (event, ctx) {}
});

Parser._registry = {};

Parser.register = function (parserName, klass) {
    Parser._registry[parserName] = klass;
    Logger.info("registered parser: " + parserName);
};

Parser.create = function (schema) {
    var parserName = schema.getParser();
    var klass = Parser._registry[parserName];
    if (klass == undefined)
        return null;
    Logger.info("creating parser: " + parserName);
    return new klass();
};

// abstarct class
var Frontend = Class({
    // updatePreedit: function (preeditText, selStart, selEnd) {},
    // updateCandidates: function (candidateList) {}, 
    // commit: function (commitText) {}, 
    // submit: function () {}
});

Frontend.register = function (klass) {
    Frontend._impl = klass;
    Logger.info("registered frontend");
}

Frontend.create = function () {
    Logger.info("creating frontend");
    return new Frontend._impl();
}

// abstarct class
var Backend = Class({
    // loadSchemaList: function (callback) {},
    // loadConfig: function (schemaName, callback) {},
    // query: function (input, callback) {}
});

Backend.register = function (klass) {
    Backend._impl = klass;
    Logger.info("registered backend");
}

Backend.create = function () {
    Logger.info("creating backend");
    return new Backend._impl();
}

var Schema = Class({
    // TODO
});

var Context = Class({
    // TODO
});

var Engine = Class({

    // TODO
    initialize: function (schema, frontend, backend) {
        Logger.debug("Engine.initialize");
        this._schema = schema;
        this._frontend = frontend;
        this._backend = backend;
    }

});

