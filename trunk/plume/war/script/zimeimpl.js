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
    CONFIG: "Config.json",

    loadSchemaList: function (callback) {
        $.getJSON(this.DATA_DIR + this.SCHEMA_LIST, null, callback);
    },

    loadConfig: function (schemaName, callback) {
        $.getJSON(this.DATA_DIR + schemaName + this.CONFIG, null, callback);
    },

    // TODO
    query: function (input, callback) {}
});

Backend.register(JSONFileBackend);

var JSFrontend = Class.extend(Frontend, {
    initialize: function () {
        Logger.debug("JSFrontend.initialize");
        this._backend = Backend.create();
        this._schemaList = [];
        var me = this;
        this._backend.loadSchemaList(function (schemaList) {
        	// debug code starts
            var cont = $('<div id="schemaList" />');
            cont.append($('<b>schemaList:</b><hr />'));
            $.each(schemaList, function (i, item) {
                var p = $('<p />').text (item.schema + " - " + item.displayName);
                cont.append(p);
            });
            $("body").append(cont);
            // debug code ends
            me._schemaList = schemaList;
            // TODO: call GWTFrontend.onSchemaListReady()
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
            // TODO: call GWTFrontend.onSchemaReady()
        });
    },
    // updatePreedit: function (preeditText, selStart, selEnd) {},
    // updateCandidates: function (candidateList) {}, 
    // commit: function (commitText) {}, 
    // submit: function () {}	
});

Frontend.register(JSFrontend);