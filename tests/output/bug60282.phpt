--TEST--
Bug #60282 (Segfault when using ob_gzhandler() with open buffers)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip Zlib extension required"; ?>
--FILE--
<?php

function fn1694425154()
{
    ob_start();
    ob_start();
    ob_start('ob_gzhandler');
    echo "done";
}
fn1694425154();
--EXPECT--
done
