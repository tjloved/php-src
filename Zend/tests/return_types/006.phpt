--TEST--
Return type allowed in child when parent does not have return type

--FILE--
<?php

class Comment
{
}
class CommentsIterator extends ArrayIterator implements Iterator
{
    function current() : Comment
    {
        return parent::current();
    }
}
function fn1945183323()
{
    $comments = new CommentsIterator([new Comment()]);
    foreach ($comments as $comment) {
        var_dump($comment);
    }
}
fn1945183323();
--EXPECTF--
object(Comment)#%d (%d) {
}
