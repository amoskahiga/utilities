/***************************************************************************
* Copyright (C) 2011 Amos Kariuki <amoskahiga@hotmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the *
* Free Software Foundation, Inc., *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA .
***************************************************************************/

#include <QDataStream>
#include <QDebug>
#include <QFile>

/**
 * Converts a unicode codepoint to a UTF8 QByteArray
 *
 * @param codepoint to convert. Expected range is 0x0 - 0x01FFFF.
 * @return a QByteArray containing 1, 2, or 3 bytes of the UTF-8 encoded codepoint.
 *         A 0 byte QByteArray is returned in an invalid codepoint was specified.
 */
QByteArray unicodeToUTF8(unsigned int codePoint)
{
  QByteArray result;

  if (codePoint <= 0x007F) {
    unsigned char byte1 = codePoint;
    result.append(byte1);
  }
  else if (codePoint <= 0x07FF) {
    unsigned char byte1 = 0b11000000 | (codePoint >> 6);
    unsigned char byte2 = 0b10000000 | (codePoint & 0b00111111);
    result.append(byte1);
    result.append(byte2);
  }
  else if (codePoint <= 0xFFFF) {
    unsigned char byte1 = 0b11100000 | (codePoint >> 12);
    unsigned char byte2 = 0b10000000 | (codePoint >> 06 & 0b00111111);
    unsigned char byte3 = 0b10000000 | (codePoint >> 00 & 0b00111111);
    result.append(byte1);
    result.append(byte2);
    result.append(byte3);
  }
  else if (codePoint <= 0x10FFFF) {
    unsigned char byte1 = 0b11110000 | (codePoint >> 18);
    unsigned char byte2 = 0b10000000 | (codePoint >> 12 & 0b00111111);
    unsigned char byte3 = 0b10000000 | (codePoint >> 06 & 0b00111111);
    unsigned char byte4 = 0b10000000 | (codePoint >> 00 & 0b00111111);
    result.append(byte1);
    result.append(byte2);
    result.append(byte3);
    result.append(byte4);
  }

  return result;
}

/**
 * Test application to write out all the UTF-8 characters, line-by-line, to an "output.dat" file.
 */
int main(int argc, char* argv[])
{
  QFile file("output.dat");
  file.open(QIODevice::WriteOnly);
  QDataStream out(&file);

  for (unsigned int i = 0; i <= 0x10FFFF; ++i) {
    QByteArray utf8Char = unicodeToUTF8(i);
    out.writeRawData(utf8Char.data(), utf8Char.size());
    out << (qint8)'\n';
  }

  file.close();
}
