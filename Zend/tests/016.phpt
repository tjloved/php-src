--TEST--
isset() with object properties when operating on non-object
--FILE--
<?php

function fn1953274702()
{
    $foo = NULL;
    isset($foo->bar->bar);
    echo "Done\n";
}
fn1953274702();
--EXPECT--	
Done
