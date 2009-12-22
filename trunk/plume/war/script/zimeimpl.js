// zimeimpl.js

var RomanParser = Class.extend(Parser, {
});

Parser.register("roman", RomanParser);

var JSONFileBackend = Class.extend(Backend, {
});

Backend.register(JSONFileBackend);
