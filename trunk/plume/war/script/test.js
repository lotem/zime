// test.js

var TestFrontend = Class.extend(Frontend, {

    initialize: function () {
        Logger.debug("TestFrontend.initialize");
        this._backend = Backend.create();
        this._schemaList = [];
        var me = this;
        this._backend.loadSchemaList(function (schemaList) {
            var cont = $('<div id="schemaList" />');
            cont.append($('<b>schemaList:</b><hr />'));
            $.each(schemaList, function (i, item) {
                var p = $('<p />').text (item.schema + " - " + item.displayName);
                cont.append(p);
            });
            $("body").append(cont);
            me._schemaList = schemaList;
        });
    },

    updatePreedit: function (preeditText, selStart, selEnd) {
        Logger.debug("TestFrontend.updatePreedit: " + preeditText + " [" + selStart + ", " + selEnd + ")");
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
        this._backend.loadSchema(schemaName, function (schema) {
            me.engine = new Engine(schema, me, me._backend);
            testAll(me);
        }, "_sync");
    }
    
});

Frontend.register(TestFrontend);

// on page load
$(function () {
    //testAjax();
    testBootstrap();
});

function testAjax() {
    var cont = $('<div id="testAjax" />');
    cont.append($('<b>testAjax:</b><hr />'));
    $.getJSON("script/sample.json", null, function (data) {
        $.each(data, function (i, item) {
            var p = $('<p />').text (item.schema + " - " + item.displayName);
            cont.append(p);
        });
        $("body").append(cont);
    });
}

function testBootstrap() {
    var t = Frontend.create();
    t.loadSchema('Pinyin');
}

function testAll(t) {
    Logger.debug("bootstrap completed");
    Logger.debug("active schema: " + t.engine.schema.schemaName);
    //testConfig(t);
    //testSchema(t);
    //testSegmentation(t);
    //testQuery(t);
    testAutoDelimit(t);
}

function testConfig(t) {
    var cont = $('<div id="testConfig" />');
    cont.append($('<b>keywords:</b><hr />'));
    var a = t.engine.schema.keywords;
    for (var k in a) {
        cont.append($('<span />').text (k + " --> " + a[k]));
        cont.append($('<br />'));
    }
    cont.append($('<b>fuzzyMap:</b><hr />'));
    var a = t.engine.schema.fuzzyMap;
    for (var k in a) {
        cont.append($('<span />').text (k + " --> " + a[k]));
        cont.append($('<br />'));
    }
    $("body").append(cont);
}

function testSchema(t) {
    var s = t.engine.schema;
    var cont = $('<div id="testSchema" />');
    cont.append($('<b>testSchema:</b><hr />'));
    cont.append($('<p />').text("parserName: " + s.getConfigValue("Parser")));
    cont.append($('<p />').text("punct:"));
    var ul = $('<ul />');
    $.each(s.getConfigList("Punct"), function (i, e) {
        ul.append($('<li />').text(e));
    });
    cont.append(ul);
    $("body").append(cont);
}

function testSegmentation(t) {
    Logger.info("testSegmentation:");
    t.engine.ctx.edit("henancyuan".split(""));
    var s = t.engine.ctx._segmentation;
    Logger.log("m: " + s.m + " n: " + s.n);
    Logger.log("a:");
    for (var i = 0; i < s.b.length; ++i) {
        for (var j = i + 1; j <= s.b.length; ++j) {
            var ki = s.b[i];
            var kj = s.b[j];
            if (s.a[kj] && s.a[kj][ki]) {
                Logger.log("[" + ki + ", " + kj + "): " + s.a[kj][ki]);
            }
        }
    }
    Logger.log("b: " + s.b);
    Logger.log("d: " + s.d);
}

function testQuery(t) {
    Logger.info("testQuery:");
    var ctx = t.engine.ctx;
    //ctx.edit("shuangzie\'gun".split(""));
    ctx.edit("henancyuan".split(""));
    ctx._backend.query(ctx, function () {
        var a = ctx.phrase;
        if (!a) {
            Logger.error("no query result");
            return;
        }
        Logger.log("phrase: ");
        for (var i = 0; i < a.length; ++i) {
            if (a[i]) {
                var b = a[i];
                for (var j = 0; j < b.length; ++j) {
                    if (b[j]) {
                        var c = b[j];
                        for (var k = 0; k < c.length; ++k) {
                            if (k >= 5) {
                                Logger.log("[" + i + ", " + j + ") ...");
                                break;
                            }
                            else
                                Logger.log("[" + i + ", " + j + ") " + c[k].text);
                        }
                    }
                }
            }
        }
        Logger.log("prediction: ");
        var d = ctx.prediction;
        for (var i = 0; i < d.length; ++i) {
            if (d[i]) {
                Logger.log(i + ": " + d[i].text);
            }
        }
    });
}

function testAutoDelimit(t) {
    Logger.info("testAutoDelimit:");
    var ctx = t.engine.ctx;
    ctx.edit("pspspsp".split(""));
    ctx.edit("anannannanspspspsp".split(""));
}
