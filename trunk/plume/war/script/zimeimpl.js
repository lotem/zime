// zimeimpl.js

var RomanParser = Class.extend(Parser, {
    // TODO
    clear: function () {},
    getPrompt: function () {},
    processInput: function (event, ctx) {}
});

Parser.register("roman", RomanParser);

var JSONFileBackend = Class.extend(Backend, {

    DATA_DIR: "data/",
    SCHEMA_LIST: "SchemaList.json",

    loadSchemaList: function (callback) {
        $.getJSON(this.DATA_DIR + this.SCHEMA_LIST, null, callback);
    },

    // TODO
    loadConfig: function (schemaName, callback) {
        callback(null);
    },

    query: function (input, callback) {}
});

Backend.register(JSONFileBackend);
