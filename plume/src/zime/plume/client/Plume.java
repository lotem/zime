package zime.plume.client;

import zime.plume.client.impl.GWTFrontend;
import com.google.gwt.core.client.EntryPoint;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Plume implements EntryPoint {

    @Override
    public void onModuleLoad() {
    	new GWTFrontend();
    }

}
