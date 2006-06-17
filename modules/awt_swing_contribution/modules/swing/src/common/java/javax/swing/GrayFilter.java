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
 * @author Anton Avtamonov
 * @version $Revision$
 */

package javax.swing;

import java.awt.Color;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.FilteredImageSource;
import java.awt.image.RGBImageFilter;

public class GrayFilter extends RGBImageFilter {
    private final ColorTransformationStrategy strategy;
    private final float interval;

    private static final float INTERVAL_BOUND = 1f / 3;

    public static Image createDisabledImage(final Image i) {
        FilteredImageSource fis = new FilteredImageSource(i.getSource(), new GrayFilter(true, 50));
        return Toolkit.getDefaultToolkit().createImage(fis);
    }

    public GrayFilter(final boolean bright, final int grayPercentage) {
        if (grayPercentage < 0 || grayPercentage > 100) {
            throw new IllegalArgumentException("Incorrect range for grayscale factor");
        }

        canFilterIndexColorModel = true;

        if (bright) {
            strategy = new BrightingColorTransformationStrategy(grayPercentage / 100f);
        } else {
            strategy = new NormalColorTransformationStrategy();
        }

        this.interval = INTERVAL_BOUND * (100 - grayPercentage) / 100f;
    }

    public int filterRGB(final int x, final int y, final int rgb) {
        int alpha = (rgb >> 24) & 0xFF;
        int r = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int b = rgb & 0xFF;

        // The formula is the ordinary transformation from colored to grayscale.
        // Taken from http://www.netnam.vn/unescocourse/computervision/comp_frm.htm (Color Representation article).
        double intensity = (0.299 * r + 0.587 * g + 0.114 * b) / 255;
        int filteredRGB = strategy.getRGB((float)intensity);
        alpha = alpha << 24;
        filteredRGB = (filteredRGB & 0x00FFFFFF) | alpha;

        return filteredRGB;
    }

    private interface ColorTransformationStrategy {
        int getRGB(float brightness);
    }

    private class BrightingColorTransformationStrategy implements ColorTransformationStrategy {
        private final float lowBound;

        public BrightingColorTransformationStrategy(final float lowBound) {
            this.lowBound = lowBound;
        }

        public int getRGB(final float brightness) {
            float normalizedBrightness = lowBound + brightness * interval;
            return Color.HSBtoRGB(0, 0, normalizedBrightness);
        }
    }

    private class NormalColorTransformationStrategy extends BrightingColorTransformationStrategy {
        public NormalColorTransformationStrategy() {
            super(0);
        }
    }
}

