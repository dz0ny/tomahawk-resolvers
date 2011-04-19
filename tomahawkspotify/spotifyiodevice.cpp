/*
 *    Copyright (c) 2011 Leo Franchi <leo@kdab.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include "spotifyiodevice.h"
#include "callbacks.h"

SpotifyIODevice::SpotifyIODevice( QObject* parent )
    : QIODevice( parent )
    , curSum( 0 )
{

}

SpotifyIODevice::~SpotifyIODevice()
{
    if( isOpen() )
        close();
}

qint64 SpotifyIODevice::readData( char* data, qint64 maxlen )
{
    qint64 written = 0;
    while( !m_audioData.isEmpty() && written < maxlen ) {
        qint64 next = m_audioData.front().second;
        if( written + next >= maxlen )
            return written;

        char* d = m_audioData.dequeue().first;
        written += next;
        memcpy( data, d, next );

        curSum -= next;
        free( d );
    }

    Q_ASSERT( curSum == 0 );

    return written;
}

qint64 SpotifyIODevice::writeData( const char* data, qint64 len )
{
    m_audioData.enqueue( QPair< char*, qint64 >( (char*)data, len ) );
    curSum += len;

    emit readyRead();
}

qint64 SpotifyIODevice::bytesAvailable() const
{
    return curSum;
}


#include "spotifyiodevice.moc"