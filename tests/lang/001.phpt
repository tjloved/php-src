--TEST--
Simple If condition test
--FILE--
<?php

function fn171709675()
{
    $a = 1;
    if ($a > 0) {
        echo "Yes";
    }
}
fn171709675();
--EXPECT--
Yes
