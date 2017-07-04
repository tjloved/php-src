--TEST--
output buffering - ob_flush
--FILE--
<?php

function fn776022956()
{
    ob_start();
    echo "foo\n";
    ob_flush();
    echo "bar\n";
    ob_flush();
}
fn776022956();
--EXPECT--
foo
bar
