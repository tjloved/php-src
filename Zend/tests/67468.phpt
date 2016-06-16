--TEST--
Bug #67468 (Segfault in highlight_file()/highlight_string())
--SKIPIF--
<?php if(!function_exists("leak")) print "skip only for debug builds"; ?>
--FILE--
<?php

function fn1591018452()
{
    highlight_string("<?php __CLASS__;", true);
    echo "done";
}
fn1591018452();
--EXPECT--
done
