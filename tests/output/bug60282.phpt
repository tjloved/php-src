--TEST--
Bug #60282 (Segfault when using ob_gzhandler() with open buffers)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip Zlib extension required"; ?>
--FILE--
<?php

function fn1250794813()
{
    ob_start();
    ob_start();
    ob_start('ob_gzhandler');
    echo "done";
}
fn1250794813();
--EXPECT--
done
