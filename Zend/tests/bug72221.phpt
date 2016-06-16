--TEST--
Bug #72221 (Segmentation fault in stream_get_line / zend_memnstr_ex)
--FILE--
<?php

function fn1869086977()
{
    $fp = fopen("php://memory", "r+");
    fwrite($fp, str_repeat("baad", 1024 * 1024));
    rewind($fp);
    stream_get_line($fp, 1024 * 1024 * 2, "aaaa");
    echo "Done\n";
}
fn1869086977();
--EXPECT--
Done
