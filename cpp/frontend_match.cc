// frontend_match.cc: manual port of frontend/match

#include "frontend_match.h"

// This order is required to get it to compile, despite clang-format
// clang-format off
#include "osh-types.h"
#include "id.h"
#include "osh-lex.h"
// clang-format on

#ifdef DUMB_ALLOC
#include "dumb_alloc.h"
#define malloc dumb_malloc
#define free dumb_free
#endif

namespace match {

Tuple2<Id_t, int> OneToken(lex_mode_t lex_mode, Str* line, int start_pos) {
  int id;
  int end_pos;
  // cstring-TODO.  line->data_ isn't necessarily safe, e.g. for eval "$x"

  // TODO: get rid of these casts
  MatchOshToken(static_cast<int>(lex_mode),
                reinterpret_cast<const unsigned char*>(line->data_), line->len_,
                start_pos, &id, &end_pos);
  return Tuple2<Id_t, int>(static_cast<Id_t>(id), end_pos);
}

Tuple2<Id_t, Str*> SimpleLexer::Next() {
  int id;
  int end_pos;
  match_func_(reinterpret_cast<const unsigned char*>(s_->data_), s_->len_, pos_,
              &id, &end_pos);
  // cstring-TODO: Don't allocate?
  // Str* val = new Str(s->data_ + pos_, end_pos - pos_);

  int len = end_pos - pos_;
  char* buf = static_cast<char*>(malloc(len + 1));
  memcpy(buf, s_->data_ + pos_, len);  // copy the list item
  buf[len] = '\0';
  Str* val = new Str(buf, len);

  pos_ = end_pos;
  return Tuple2<Id_t, Str*>(static_cast<Id_t>(id), val);
}

SimpleLexer* BraceRangeLexer(Str* s) {
  return new SimpleLexer(&MatchBraceRangeToken, s);
}

SimpleLexer* GlobLexer(Str* s) {
  return new SimpleLexer(&MatchGlobToken, s);
}

SimpleLexer* EchoLexer(Str* s) {
  return new SimpleLexer(&MatchEchoToken, s);
}

bool IsValidVarName(Str* s) {
  // Call generated function.  Note: this relies on operator overloading.
  return ::IsValidVarName(reinterpret_cast<const unsigned char*>(s->data_),
                          s->len_);
}

int MatchOption(Str* s) {
  int id;
  ::MatchOption(reinterpret_cast<const unsigned char*>(s->data_), s->len_, &id);
  return id;
}

}  // namespace match
