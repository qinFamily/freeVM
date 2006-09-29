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
 * @author Oleg V. Khaschansky
 * @version $Revision$
 */
/*
 * Created on 20.01.2005
 */
package org.apache.harmony.awt.gl.image;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;

public class FileDecodingImageSource extends DecodingImageSource {
  String filename;

  public FileDecodingImageSource(String file) {
    SecurityManager security = System.getSecurityManager();
    if (security != null) {
        security.checkRead(file);
    }

    filename = file;
  }

  protected boolean checkConnection() {
      SecurityManager security = System.getSecurityManager();
      if (security != null) {
          try {
            security.checkRead(filename);
          } catch (SecurityException e) {
              return false;
          }
      }

      return true;
  }

  protected InputStream getInputStream() {
    try {
      return new BufferedInputStream(new FileInputStream(filename));
    } catch (FileNotFoundException e) {
      return null;
    }
  }

}
