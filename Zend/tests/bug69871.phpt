--TEST--
Bug #69871 (Short-circuiting failure with smart_branch)
--INI--
opcache.enable=0
--FILE--
<?php

function fn1153695053()
{
    $a = true;
    if (isset($a) && 0) {
        var_dump(true);
    } else {
        var_dump(false);
    }
}
fn1153695053();
--EXPECT--
bool(false)
