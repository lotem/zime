package zime.plume.client.impl;

import zime.plume.client.KeyEventHandler;
import zime.plume.client.Parser;
import zime.plume.client.ParserFactory;
import zime.plume.client.Schema;

public class RomanParserFactory implements ParserFactory {

    @Override
    public Parser create(Schema schema, KeyEventHandler fallback) {
        return new RomanParser(schema, fallback);
    }

    @Override
    public String getName() {
        return "roman";
    }

}
