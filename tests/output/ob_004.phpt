--TEST--
output buffering - ob_clean
--FILE--
<?php

function fn765287620()
{
    ob_start();
    echo "foo\n";
    ob_clean();
    echo "bar\n";
}
fn765287620();
--EXPECT--
bar
