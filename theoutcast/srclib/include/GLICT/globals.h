/*
    GLICT - Graphics Library Interface Creation Toolkit
    Copyright (C) 2006-2007 OBJECT Networks

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <GLICT/container.h>
#include <GLICT/skinner.h>
#include <GLICT/types.h>


typedef void (*GLICTDEBUGCALLBACKPROC )(unsigned int len, const char *string);

/// Stores some global settings.
class glictGlobalsClass {
    public:
        glictGlobalsClass();
        ~glictGlobalsClass();

        float windowTitleBgColor[4]; float windowTitleColor[4]; glictSkinner *windowTitleSkin;
        glictSkinner *windowBodySkin;
        glictSkinner *buttonSkin, *buttonHighlightSkin;
        glictSkinner *textboxSkin;
        glictColor buttonTextColor, buttonHighlightTextColor;
        glictColor panelTextColor;
        glictColor textboxTextColor;

        float w,h;
        glictPos lastMousePos; // last coordinates where user clicked
        glictClippingMode clippingMode;
        glictContainer* topFocused;

		GLICTDEBUGCALLBACKPROC debugCallback;

};
extern glictGlobalsClass glictGlobals;

#define GLICT_RENDERING false
#define GLICT_SELECTING true
