--TEST--
Bug #67468 (Segfault in highlight_file()/highlight_string())
--SKIPIF--
<?php if(!function_exists("leak")) print "skip only for debug builds"; ?>
--FILE--
<?php

function fn910670315()
{
    highlight_string("<?php __CLASS__;", true);
    echo "done";
}
fn910670315();
--EXPECT--
done
