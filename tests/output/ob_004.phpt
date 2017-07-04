--TEST--
output buffering - ob_clean
--FILE--
<?php

function fn1669939143()
{
    ob_start();
    echo "foo\n";
    ob_clean();
    echo "bar\n";
}
fn1669939143();
--EXPECT--
bar
