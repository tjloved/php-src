--TEST--
Bug #30726 (-.1 like numbers are not being handled correctly)
--FILE--
<?php

function fn344162079()
{
    echo (int) is_float('-.1' * 2), "\n";
}
fn344162079();
--EXPECT--
1
