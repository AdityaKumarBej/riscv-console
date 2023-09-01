#include "GUITextViewGTK3.h"

CGUITextViewGTK3::CGUITextViewGTK3(GtkWidget *widget, bool reference) : CGUIContainerGTK3(widget,reference){

}

CGUITextViewGTK3::~CGUITextViewGTK3(){

}

void CGUITextViewGTK3::PlaceCursorOnscreen(){
    gtk_text_view_place_cursor_onscreen(GTK_TEXT_VIEW(DWidget));
}

void CGUITextViewGTK3::SetEditable(bool edit){
    gtk_text_view_set_editable(GTK_TEXT_VIEW(DWidget), edit);
}

void CGUITextViewGTK3::SetCursorVisible(bool visibility){
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(DWidget), visibility);
}

void CGUITextViewGTK3::SetMonospace(bool monospace){
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(DWidget), monospace);
}

void CGUITextViewGTK3::SetWrapMode(EWrapModeType wrap){
    GtkWrapMode WrapMode = GTK_WRAP_NONE;
    switch(wrap){
        case CGUITextView::EWrapModeType::None:             WrapMode = GTK_WRAP_NONE;
                                                            break;
        case CGUITextView::EWrapModeType::Character:        WrapMode = GTK_WRAP_CHAR;
                                                            break;
        case CGUITextView::EWrapModeType::Word:             WrapMode = GTK_WRAP_WORD;
                                                            break;
        case CGUITextView::EWrapModeType::WordCharacter:    WrapMode = GTK_WRAP_WORD_CHAR;
                                                            break;
        default:                                            break;
    }
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(DWidget), WrapMode);
}


std::shared_ptr<CGUITextBuffer> CGUITextViewGTK3::GetBuffer(){
    if(!DBuffer){
        DBuffer = std::make_shared<CGUITextBufferGTK3>(gtk_text_view_get_buffer(GTK_TEXT_VIEW(DWidget)));
    }
    return DBuffer;
}

int CGUITextViewGTK3::GetRequiredCharactersWidth(int chars){
    GtkTextAttributes *TextAttributes = gtk_text_view_get_default_attributes(GTK_TEXT_VIEW(DWidget));
    PangoLayout *CharacterCell = gtk_widget_create_pango_layout(DWidget, "X");
    PangoRectangle LogicalRect;
    pango_layout_get_pixel_extents(CharacterCell, NULL, &LogicalRect);
    int WidgetWidth;
    WidgetWidth = chars <= 0 ? -1 : LogicalRect.width * chars + TextAttributes->left_margin +  TextAttributes->right_margin;
    g_object_unref(CharacterCell);
    gtk_text_attributes_unref(TextAttributes);
    return WidgetWidth;
}

int CGUITextViewGTK3::GetRequiredLinesHeight(int lines){
    GtkTextAttributes *TextAttributes = gtk_text_view_get_default_attributes(GTK_TEXT_VIEW(DWidget));
    PangoLayout *CharacterCell = gtk_widget_create_pango_layout(DWidget, "X");
    PangoRectangle LogicalRect;
    pango_layout_get_pixel_extents(CharacterCell, NULL, &LogicalRect);
    int WidgetHeight;
    WidgetHeight = lines <= 0 ? -1 : (LogicalRect.height + TextAttributes->pixels_above_lines + TextAttributes->pixels_below_lines) * lines;
    g_object_unref(CharacterCell);
    gtk_text_attributes_unref(TextAttributes);
    return WidgetHeight;
}

int CGUITextViewGTK3::GetLineNumberAtY(int y){
    GtkTextIter TempIter;
    gtk_text_view_get_line_at_y(GTK_TEXT_VIEW(DWidget), &TempIter, y, NULL);
    return gtk_text_iter_get_line(&TempIter);
}