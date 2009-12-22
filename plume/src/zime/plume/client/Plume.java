package zime.plume.client;

import zime.plume.client.impl.RomanParserFactory;
import zime.plume.client.impl.StaticFileBackend;
import zime.plume.client.impl.WebFrontend;

import com.google.gwt.core.client.EntryPoint;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Plume implements EntryPoint {

    @Override
    public void onModuleLoad() {
        // TODO: register parser factories
        Parser.register(new RomanParserFactory());
        // TODO: initiate front-end & back-end
        new WebFrontend(new StaticFileBackend()).initialize();
    }

}
