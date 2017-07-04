--TEST--
output buffering - ob_start
--FILE--
<?php

function fn1313494023()
{
    ob_start();
    echo "foo\n";
}
fn1313494023();
--EXPECT--
foo
