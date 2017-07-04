--TEST--
Bug #44660 (Indexed and reference assignment to propery of non-object don't trigger warning)
--FILE--
<?php

function fn506935888()
{
    $s = "hello";
    $a = true;
    echo "--> read access: ";
    echo $a->p;
    echo "\n--> direct assignment: ";
    $a->p = $s;
    echo "\n--> increment: ";
    $a->p++;
    echo "\n--> reference assignment:";
    $a->p =& $s;
    echo "\n--> reference assignment:";
    $s =& $a->p;
    echo "\n--> indexed assignment:";
    $a->p[0] = $s;
    echo "\n--> Confirm assignments have had no impact:\n";
    var_dump($a);
}
fn506935888();
--EXPECTF--
--> read access: 
Notice: Trying to get property of non-object in %sbug44660.php on line %d

--> direct assignment: 
Warning: Attempt to assign property of non-object in %sbug44660.php on line %d

--> increment: 
Warning: Attempt to increment/decrement property of non-object in %sbug44660.php on line %d

--> reference assignment:
Warning: Attempt to modify property of non-object in %sbug44660.php on line %d

--> reference assignment:
Warning: Attempt to modify property of non-object in %sbug44660.php on line %d

--> indexed assignment:
Warning: Attempt to modify property of non-object in %sbug44660.php on line %d

--> Confirm assignments have had no impact:
bool(true)
