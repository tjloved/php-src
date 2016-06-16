--TEST--
output buffering - error message nirvana bug #37714
--SKIPIF--
<?php
if (!extension_loaded("zlib")) die("skip need ext/zlib");
?>
--ENV--
HTTP_ACCEPT_ENCODING=gzip,deflate
--INI--
display_errors=1
zlib.output_compression=1
--FILE--
<?php

function fn1727950276()
{
    ob_start('ob_gzhandler');
}
fn1727950276();
--EXPECTF--
‹%a
