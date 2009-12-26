/**
 * 
 */
package zime.plume.client;

import com.google.gwt.core.client.JavaScriptObject;

public class SchemaListItem extends JavaScriptObject {
	protected SchemaListItem() {}
	public final native String getDisplayName() /*-{ return this.displayName; }-*/;
	public final native String getSchemaName() /*-{ return this.schema; }-*/;
}