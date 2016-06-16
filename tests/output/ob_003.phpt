--TEST--
output buffering - ob_flush
--FILE--
<?php

function fn1184387642()
{
    ob_start();
    echo "foo\n";
    ob_flush();
    echo "bar\n";
    ob_flush();
}
fn1184387642();
--EXPECT--
foo
bar
