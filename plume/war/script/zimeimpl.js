// zimeimpl.js

var RomanParser = Class.extend(Parser, {
    // TODO
    clear: function () {},
    getPrompt: function () {},
    processInput: function (event, ctx) {}
});

Parser.register("roman", RomanParser);

var JSONFileBackend = Class.extend(Backend, {
    // TODO
    loadSchemaList: function (callback) {},
    loadConfig: function (schemaName, callback) {},
    query: function (input, callback) {}
});

Backend.register(JSONFileBackend);
