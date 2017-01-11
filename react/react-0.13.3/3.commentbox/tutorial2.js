// tutorial2.js

var Comment = React.createClass({
  render: function() {
    return (
      <div className="comment">
        <h2 className="commentAuthor">
          {this.props.author}
        </h2>
         { this.props.children }
      </div>
    );
  }
});

var CommentList = React.createClass({
  render: function() {
    return (
      <div className="commentList">
        <Comment author="Pete Hunt">This is <b>one</b> comment</Comment>
        <Comment author="Jordan Walke">This is *another* comment</Comment>
      </div>
    );
  }
});

var CommentForm = React.createClass({
  render: function() {
    return (
      <div className="commentForm">
        Hello, world! I am a CommentForm.
      </div>
    );
  }
});

var data = [
  {author: "Pete Hunt2", text: "This is one comment2"},
  {author: "Jordan Walke2", text: "This is *another* comment2"}
];

var CommentList2 = React.createClass({
  render: function() {
    var commentNodes = this.props.data.map(function (comment) {
      return (
        <Comment author={comment.author}>
          {comment.text}
        </Comment>
      );
    });
    return (
      <div className="commentList">
        {commentNodes}
      </div>
    );
  }
});

var CommentBox = React.createClass({
  render: function() {
    return (
      <div className="commentBox">
        <h1>Comments</h1>
		<CommentList2 data={this.props.data} />
        <CommentList />
        <CommentForm />
      </div>
    );
  }
});

React.render(
  <div> 
	<CommentBox data={data}/>
  </div>,
  document.getElementById('content')
);
