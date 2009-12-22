// zimecore.js

var Class = function (proto) {
        var klass = function () {
                if (typeof this.initialize == "function") {
                        this.initialize.apply(this, arguments);
                }
        };
        $.extend(klass, this);
        if (proto != undefined) {
                klass.prototype = proto;
                klass.prototype.constructor = klass;
        }
        return klass;
};

Class.extend = function (klass, extension) {
    return new Class($.extend({}, klass.prototype, extension));
};

var Schema = Class({
});

var Parser = Class({
    // promptText: "",
    // clear: function () {},
    // processInput: function (event, ctx) {}
});

Parser._registry = {};

Parser.register = function (parserName, klass) {
    Parser._registry[parserName] = klass;
};

Parser.create = function (schema) {
    var parserName = schema.getParser();
    var klass = Parser._registry[parserName];
    if (klass == undefined)
        return null;
    return new klass();
}

var Frontend = Class({
    // updatePreedit: function (preeditText, selStart, selEnd) {},
    // updateCandidates: function (candidateList) {}, 
    // commit: function (commitText) {}, 
    // submit: function () {}
});

Frontend.register = function (klass) {
    Frontend._impl = klass;
}

Frontend.create = function () {
    return new Frontend._impl();
}

var Backend = Class({
    // loadSchemaList: function (callback) {},
    // loadConfig: function (schemaName, callback) {},
    // query: function (input, callback) {}
});

Backend.register = function (klass) {
    Backend._impl = klass;
}

Backend.create = function () {
    return new Backend._impl();
}

var Context = Class({
});

var Engine = Class({
});

