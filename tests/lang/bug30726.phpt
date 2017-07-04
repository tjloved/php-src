--TEST--
Bug #30726 (-.1 like numbers are not being handled correctly)
--FILE--
<?php

function fn289446833()
{
    echo (int) is_float('-.1' * 2), "\n";
}
fn289446833();
--EXPECT--
1
