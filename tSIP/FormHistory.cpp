//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Clipbrd.hpp"
#include "FormHistory.h"
#include "History.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmHistory *frmHistory;
//---------------------------------------------------------------------------
__fastcall TfrmHistory::TfrmHistory(TComponent* Owner, History *history,
	CallbackCall callbackCall,
	CallbackPhonebookEdit callbackPhonebookEdit,
	CallbackHttpQuery callbackHttpQuery
	)
	: TForm(Owner), history(history),
	callbackCall(callbackCall),
	callbackPhonebookEdit(callbackPhonebookEdit),
	callbackHttpQuery(callbackHttpQuery),
	updateNeeded(false), updating(false),
	usePaiForDisplayIfAvailable(true),
	usePaiForDialIfAvailable(true)
{
	assert(history);
	assert(callbackCall);
	assert(callbackPhonebookEdit);
	assert(callbackHttpQuery);
	history->addObserver(*this);
}
//---------------------------------------------------------------------------

void TfrmHistory::FilterHistory(void)
{
	FilteredEntry fentry;
	AnsiString needle = UpperCase(edFilter->Text);
	filteredEntries.clear();
	const std::deque<History::Entry>& entries = history->GetEntries();
	if (needle == "")
	{
		for (int i=0; i<entries.size(); i++)
		{
			const History::Entry& entry = entries[i];
			fentry.id = i;
			fentry.entry = entry;
			filteredEntries.push_back(fentry);
		}
	}
	else
	{
		for (int i=0; i<entries.size(); i++)
		{
			const History::Entry& entry = entries[i];
			if (
				UpperCase(entry.uri.c_str()).Pos(needle) > 0 ||
				UpperCase(entry.peerName.c_str()).Pos(needle) > 0 ||
				UpperCase(entry.contactName.c_str()).Pos(needle) > 0 ||
				UpperCase(entry.paiUri.c_str()).Pos(needle) > 0 ||
				UpperCase(entry.paiPeerName.c_str()).Pos(needle) > 0 ||
				UpperCase(entry.paiContactName.c_str()).Pos(needle) > 0
				)
			{
				fentry.id = i;
				fentry.entry = entry;
				filteredEntries.push_back(fentry);
			}
		}
	}
	lvHistory->Items->Count = filteredEntries.size();
	lvHistory->Invalidate();
}

void TfrmHistory::SetUpdating(bool state)
{
    updating = state;
	if (updateNeeded)
	{
		FilterHistory();
		updateNeeded = false;		
	}
}

void TfrmHistory::obsUpdate(Observable* o, Argument * arg)
{
#if 0
	History* history = dynamic_cast<History*>(o);
	if (history)
	{
		//HistoryNotifyArgument* darg = dynamic_cast<HistoryNotifyArgument*>(arg);
		//assert(darg);
		const std::deque<History::Entry>& entries = history->GetEntries();
		lvHistory->Items->Count = entries.size();
		lvHistory->Invalidate();
	}
#else
	/** Delay filtering if history is not visible to reduce CPU load */
	if (updating)
	{
		FilterHistory();
	}
	else
	{
    	updateNeeded = true;
	}
#endif
}

void __fastcall TfrmHistory::lvHistoryData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	const History::Entry &entry = filteredEntries[id].entry;
	int sourceId = filteredEntries[id].id;
	AnsiString ts;
	ts.sprintf("%02d  %02d:%02d:%02d",
        entry.timestamp.day,
		entry.timestamp.hour, entry.timestamp.min, entry.timestamp.sec);
	Item->Caption = ts;

	if (usePaiForDisplayIfAvailable && entry.paiUri != "")
	{
		AnsiString contactName = entry.paiContactName;
		if (contactName == "")
		{
			contactName = entry.paiPeerName;
			if (contactName == "")
			{
				contactName = entry.paiUri;
			}
		}
		Item->SubItems->Add(contactName);
	}
	else
	{
		AnsiString contactName = entry.contactName;
		if (contactName == "")
		{
			contactName = entry.peerName;
			if (contactName == "")
			{
				contactName = entry.uri;
			}
		}
		Item->SubItems->Add(contactName);
	}
	
	if (entry.incoming)
	{
		if (entry.time > 0)
			Item->ImageIndex = 0;
		else
			Item->ImageIndex = 1;
	}
	else
	{
		if (entry.time > 0)
			Item->ImageIndex = 2;
		else
			Item->ImageIndex = 3;
	}
}
//---------------------------------------------------------------------------

History::Entry* TfrmHistory::getSelectedEntry(void)
{
	TListItem *item = lvHistory->Selected;
	if (item == NULL)
	{
		return NULL;
	}
	int id = item->Index;
	return &filteredEntries[id].entry;
}

AnsiString TfrmHistory::getDefaultUri(const History::Entry* entry)
{
	assert(entry);
	if (usePaiForDisplayIfAvailable && entry->paiUri != "")
	{
		return entry->paiUri;
	}
	else
	{
		return entry->uri;
	}
}

void __fastcall TfrmHistory::lvHistoryDblClick(TObject *Sender)
{
	History::Entry* entry = getSelectedEntry();
	if (entry == NULL)
	{
		return;
	}

	AnsiString uri = getDefaultUri(entry);

	callbackCall(uri.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfrmHistory::edFilterChange(TObject *Sender)
{
	FilterHistory();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmHistory::miCopyNumberClick(TObject *Sender)
{
	History::Entry* entry = getSelectedEntry();
	if (entry == NULL)
	{
		return;
	}
	Clipboard()->AsText = getDefaultUri(entry);
}
//---------------------------------------------------------------------------

void __fastcall TfrmHistory::miAddEditPhonebookClick(TObject *Sender)
{
	History::Entry* entry = getSelectedEntry();
	if (entry == NULL)
	{
		return;
	}
	callbackPhonebookEdit(getDefaultUri(entry));
}
//---------------------------------------------------------------------------

void __fastcall TfrmHistory::miHttpQueryClick(TObject *Sender)
{
	History::Entry* entry = getSelectedEntry();
	if (entry == NULL)
	{
		return;
	}
	callbackHttpQuery(getDefaultUri(entry));
}
//---------------------------------------------------------------------------

void __fastcall TfrmHistory::edFilterKeyPress(TObject *Sender, char &Key)
{
	TListView *lv = lvHistory;
	if (Key == VK_RETURN)
	{
		if (lv->Items->Count > 0)
		{
			lv->Items->Item[0]->Selected = true;
			lv->Items->Item[0]->Focused = true;
			lv->SetFocus();
		}
	}
}
//---------------------------------------------------------------------------

void TfrmHistory::Scale(int percentage)
{
	TListView *lv = lvHistory;
	for (int i=0; i<lv->Columns->Count; i++)
	{
        lv->Columns->Items[i]->Width = (float)lv->Columns->Items[i]->Width * percentage / 100.0f;
    }
}

void TfrmHistory::UsePaiForDisplayIfAvailable(bool state)
{
	if (usePaiForDisplayIfAvailable != state)
	{
		usePaiForDisplayIfAvailable = state;
		FilterHistory();
	}
}

void TfrmHistory::UsePaiForDialIfAvailable(bool state)
{
	usePaiForDialIfAvailable = state;
}



