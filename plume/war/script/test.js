// test.js

var TestFrontend = new Class({

    initialize: function () {
        Logger.debug("TestFrontend.initialize");
        this._backend = Backend.create();
        this._schemaList = [];
        var me = this;
        this._backend.loadSchemaList(function (schemaList) {
            var cont = $('<div id="schemaList" />');
            cont.append($('<p>SchemaList:</p>'));
            $.each(schemaList, function (i, item) {
                var p = $('<p />').text (item.schema + " - " + item.displayName);
                cont.append(p);
            });
            $("body").append(cont);
            me._schemaList = schemaList;
            if (schemaList.length > 0) {
                me.loadSchema(schemaList[0].schema);
            }
        });
    },

    updatePreedit: function (preeditText, selStart, selEnd) {
        Logger.debug("TestFrontend.updatePreedit: " + preeditText + "[" + selStart + ", " + selEnd + "]");
    },

    updateCandidates: function (candidateList) {
        Logger.debug("TestFrontend.updateCandidates: " + candidateList);
    }, 

    commit: function (commitText) {
        Logger.debug("TestFrontend.commit: " + commitText);
    }, 

    submit: function () {
        Logger.debug("TestFrontend.submit");
    },

    loadSchema: function (schemaName) {
        Logger.debug("TestFrontend.loadSchema: " + schemaName);
        var me = this;
        this._backend.loadConfig(schemaName, function (config) {
            schema = new Schema (schemaName, config);
            me._engine = new Engine(schema, me, me._backend);
            testAll(me);
        });
    },
    
    input: function (text) {
        // TODO: foreach char in text:
        // e = new KeyEvent(keycode of char, mask);
        // this._engine.processKeyEvent(e);
    }

});

Frontend.register(TestFrontend);

// on page load
$(function () {
    testAjax();
    testBootstrap();
});

function testAjax() {
    var cont = $('<div id="testAjax" />');
    cont.append($('<p>testAjax:</p>'));
    $.getJSON("sample.json", null, function (data) {
        $.each(data, function (i, item) {
            var p = $('<p />').text (item.schema + " - " + item.displayName);
            cont.append(p);
        });
        $("body").append(cont);
    });
}

function testBootstrap() {
    var t = Frontend.create();
}

function testAll(t) {
    Logger.debug("bootstrap completed");
    // TODO
}

