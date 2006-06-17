/*
 *  Copyright 2005 - 2006 The Apache Software Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/**
 * @author Ilya S. Okomin
 * @version $Revision$
 */
package org.apache.harmony.awt.gl.font;

import java.awt.Font;
import java.awt.peer.FontPeer;
import java.io.File;
import java.util.Properties;
import java.util.Vector;

import org.apache.harmony.awt.gl.font.CompositeFont;
import org.apache.harmony.awt.gl.font.FontManager;
import org.apache.harmony.awt.gl.font.FontProperty;

/**
 *
 * Windows FontManager implementation.
 */
public class WinFontManager extends FontManager {

    /** Available windows charset names */
    public static final String[] WINDOWS_CHARSET_NAMES = {
            "ANSI_CHARSET", "DEFAULT_CHARSET", "SYMBOL_CHARSET", "SHIFTJIS_CHARSET",
            "GB2312_CHARSET", "HANGEUL_CHARSET", "CHINESEBIG5_CHARSET",
            "OEM_CHARSET", "JOHAB_CHARSET", "HEBREW_CHARSET", "ARABIC_CHARSET",
            "GREEK_CHARSET", "TURKISH_CHARSET", "VIETNAMESE_CHARSET", "THAI_CHARSET",
            "EASTEUROPE_CHARSET", "RUSSIAN_CHARSET", "MAC_CHARSET", "BALTIC_CHARSET"
    };
    
    /** WinFontManager singleton instanse */
    public static final WinFontManager inst = new WinFontManager();

    private WinFontManager() {
        super();
        initFontProperties();
    }

    public void initLCIDTable(){
        NativeFont.initLCIDsTable(this.tableLCID);
    }

    /**
     * Creates and returns logical font peer. 
     * 
     * @param logicalName logical font family name
     * @param font specified Font object
     * @return In case of non-null fontproperties for the specified logical 
     * name CompositeFont object is returned, otherwise WindowsFont object is 
     * returned.
     */
    private FontPeer CreateLogicalFont(String logicalName, Font font){
        FontProperty[] fps = getFontProperties(logicalName + "." + font.getStyle());
        if (fps != null){
            return new CompositeFont(font, fps, logicalName);
        }
        return new WindowsFont(font, true, logicalName, logicalName, 0);
    }

    public FontPeer createFont(Font font){
        String fontName = font.getName();
        if (isFontLogical(font)){
            return CreateLogicalFont(fontName, font);
        }

        int fontStyle = 0;
        String family = null;
        String face = null;

        int faceIndex = getFaceIndex(fontName);
        // Check if name parameter is face name
        if (faceIndex != -1){
            fontStyle = NativeFont.getFontStyle(faceIndex);
            family = NativeFont.getFamily(faceIndex);
        } else {
            return CreateLogicalFont(FontManager.LOGICAL_FONT_NAMES[3], font);  // "dialog"
        }
        return new WindowsFont(font, false, family, face, fontStyle);
    }

    public FontPeer createFont(FontProperty fp, int size){
        Font propertyFont = new Font(fp.getName(), fp.getStyle(), size);
        return createFont(propertyFont);
    }

    /**
     * Initializes fProperties array field for the current system configuration font
     * property file.
     * 
     * @return true is success, false if font property doesn't exist or doesn't
     * contain roperties. 
     */
    public boolean initFontProperties(){
        File fpFile = getFontPropertyFile();
        if (fpFile == null){
            return false;
        }

        Properties props = getProperties(fpFile);
        if (props == null){
            return false;
        }

        for (int i=0; i < LOGICAL_FONT_NAMES.length; i++){
            for (int j=0; j < STYLE_NAMES.length; j++){
                Vector propsVector = new Vector();

                // Number of entries for a logical font
                int numComp = 0;
                // Is more entries for this style and logical font name left
                boolean moreEntries = true;
                String value = null;

                while(moreEntries){
                    // Component Font Mappings property name
                    String property = FONT_MAPPING_KEYS[0].replaceAll("LogicalFontName", LOGICAL_FONT_NAMES[i]).replaceAll("StyleName", STYLE_NAMES[j]).replaceAll("ComponentIndex", String.valueOf(numComp));
                    value = props.getProperty(property);

                    // If the StyleName is omitted, it's assumed to be plain
                    if ((j == 0) && (value == null)){
                        property = FONT_MAPPING_KEYS[1].replaceAll("LogicalFontName", LOGICAL_FONT_NAMES[i]).replaceAll("ComponentIndex", String.valueOf(numComp));
                        value = props.getProperty(property);
                    }

                    if (value != null){

                        String fontName = value.substring(0, value.indexOf(","));
                        int ind = fontName.lastIndexOf("Bold");
                        if (ind != -1){
                            fontName = fontName.substring(0, ind-1);
                        } else {
                            ind = fontName.lastIndexOf("Italic");
                            if(ind != -1){
                                fontName = fontName.substring(0, ind-1);
                            }
                        }


                        String charset = value.substring(value.indexOf(",") + 1, value.length());

                        // Font File Names property value
                        String fileName = props.getProperty(FONT_FILE_NAME.replaceAll("PlatformFontName", fontName).replaceAll(" ", "_"));

                        // Exclusion Ranges property value
                        String exclString = props.getProperty(EXCLUSION_RANGES.replaceAll("LogicalFontName", LOGICAL_FONT_NAMES[i]).replaceAll("ComponentIndex", String.valueOf(numComp)));
                        int[] exclRange = parseIntervals(exclString);

                        // Component Font Character Encodings property value
                        String encoding = props.getProperty(FONT_CHARACTER_ENCODING.replaceAll("LogicalFontName", LOGICAL_FONT_NAMES[i]).replaceAll("ComponentIndex", String.valueOf(numComp)));

                        FontProperty fp = new WinFontProperty(fileName, fontName, charset, j, exclRange, encoding);
                        propsVector.add(fp);
                        numComp++;
                    } else {
                        moreEntries = false;
                    }
                }
                fProperties.put(LOGICAL_FONT_NAMES[i] + "." + j, propsVector);
            }
        }

        return true;

    }

    public int getFaceIndex(String faceName){
        for (int i=0; i<NativeFont.faces.length; i++ ){
            if (NativeFont.faces[i].equalsIgnoreCase(faceName))
                return i;
        }
        return -1;
    }

    public Font[] getAllFonts() {
        String faces[] = NativeFont.getAvailableFaces();
        int count = faces.length;
        Font[] fonts = new Font[count];

        for (int i = 0; i < count; i++){
            fonts[i] = new Font(faces[i], Font.PLAIN, 1);
        }

        return fonts;
    }

    public String[] getAllFamilies() {
        if (allFamilies == null){
            allFamilies = NativeFont.getFamilies();
        }
        return allFamilies;
    }


}
