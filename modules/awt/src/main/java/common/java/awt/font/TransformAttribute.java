/*
 *  Copyright 2005 - 2006 The Apache Software Foundation or its licensors, as applicable.
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
package java.awt.font;

import java.awt.geom.AffineTransform;
import java.io.Serializable;

public final class TransformAttribute implements Serializable {
    private static final long serialVersionUID = 3356247357827709530L;

    // affine transform of this TransformAttribute instance
    private AffineTransform fTransform;

    public TransformAttribute(AffineTransform transform) {
        if (transform == null) {
            throw new IllegalArgumentException("transform can not be null");
        }
        if (!transform.isIdentity()){
            this.fTransform = new AffineTransform(transform);
        }
    }

    public AffineTransform getTransform() {
        if (fTransform != null){
            return new AffineTransform(fTransform);
        }
        return new AffineTransform();
    }

    public boolean isIdentity() {
        return (fTransform == null);
    }

}

