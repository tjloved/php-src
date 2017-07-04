--TEST--
output buffering - ob_get_flush
--FILE--
<?php

function fn67973386()
{
    ob_start();
    echo "foo\n";
    var_dump(ob_get_flush());
}
fn67973386();
--EXPECT--
foo
string(4) "foo
"
