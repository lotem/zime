package cn.zzsst.client;

import java.util.ArrayList;


import com.google.gwt.user.client.Command;
import com.google.gwt.user.client.DOM;
import com.google.gwt.user.client.ui.DecoratedPopupPanel;
import com.google.gwt.user.client.ui.MenuBar;
import com.google.gwt.user.client.ui.MenuItem;
import com.google.gwt.user.client.ui.PopupPanel;
import com.google.gwt.user.client.ui.TextBox;

public class PreeditBox extends TextBox {
	
    private MenuBar menu;
    private PopupPanel popup;

    public PreeditBox() {
        menu = new MenuBar(true);
        popup = new DecoratedPopupPanel(true);
        popup.setWidget(menu);
        popup.setPreviewingAllNativeEvents(true);
        popup.addAutoHidePartner(this.getElement());
    }
    
	public void updateMenuItems(CandidateList candidateList) {
		ArrayList<String> cand = candidateList.getCandidates();
		final Command cmd = new Command() {
			@Override
			public void execute() {}
		};
		menu.clearItems();
		int start = candidateList.getCurrentPage() * candidateList.getPageSize();
		int count = Math.min(candidateList.getPageSize(), cand.size() - start);
		for (int i = 0; i < count; ++i) {
			String disp = "<b>" + (i + 1) + ".</b>&nbsp;" + cand.get(start + i);
			final MenuItem item = new MenuItem(disp, true, cmd);
			DOM.setStyleAttribute(item.getElement(), "whiteSpace", "nowrap");
			menu.addItem(item);
		}
	}

	public void showMenu() {
        int left = getAbsoluteLeft();
		int top = getAbsoluteTop() + getOffsetHeight();
		popup.setPopupPosition(left, top);
		popup.show();
	}

	public void hideMenu() {
		popup.hide();
	}

}
